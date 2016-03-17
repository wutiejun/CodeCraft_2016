#include <base.h>
#include <linklist.h>
#include <memory.h>
#include "memtypes.h"
#include "pqueue.h"
#include "vector.h"
#include "hash.h"
#include "sdn.h"

extern CURRENT_QUERY_T current_query;

int te_cost_cmp(QCCS_TE_COST *c1, QCCS_TE_COST *c2)
{
    int ret_val = 0;
    if (current_query.minimize_delay)
    {
        ret_val =  (int)((c1->node_delay + c1->link_delay) - (c2->node_delay +  c2->link_delay) );
        if (ret_val != 0)
        {
            return ret_val;
        }

    }
    if (current_query.minimize_hops)
    {
        ret_val = c1->number_of_hops - c2->number_of_hops;
        if (ret_val != 0)
        {
            return ret_val;
        }
    }

    ret_val =  (int)(c1->exclusion_overlap.number_of_common_srlgs - c2->exclusion_overlap.number_of_common_srlgs);
    if (ret_val != 0)
    {
        return ret_val;
    }

    ret_val =  (int)(c1->exclusion_overlap.number_of_common_routers - c2->exclusion_overlap.number_of_common_routers);
    if (ret_val != 0)
    {
        return ret_val;
    }

    ret_val =  (int)(c1->exclusion_overlap.number_of_common_links - c2->exclusion_overlap.number_of_common_links);
    if (ret_val != 0)
    {
        return ret_val;
    }

    return c1->total_te_metric_cost - c2->total_te_metric_cost;
}

int te_cost_add(QCCS_TE_COST *new_cost, QCCS_TE_COST *old_cost, QCCS_TE_COST *delta)
{
    new_cost->number_of_hops = old_cost->number_of_hops + delta->number_of_hops;
    new_cost->node_delay = old_cost->node_delay + delta->node_delay;
    new_cost->link_delay = old_cost->link_delay + delta->link_delay;
    new_cost->total_te_metric_cost = old_cost->total_te_metric_cost + delta->total_te_metric_cost;
    new_cost->exclusion_overlap.number_of_common_routers = old_cost->exclusion_overlap.number_of_common_routers + delta->exclusion_overlap.number_of_common_routers;
    new_cost->exclusion_overlap.number_of_common_links = old_cost->exclusion_overlap.number_of_common_links + delta->exclusion_overlap.number_of_common_links;
    new_cost->exclusion_overlap.number_of_common_srlgs = delta->exclusion_overlap.number_of_common_srlgs;
    return 0;
}
int te_cost_tri_cmp(QCCS_TE_COST *c1, QCCS_TE_COST *c2, QCCS_TE_COST *delta)
{
    QCCS_TE_COST c3;
    memset(&c3, 0, sizeof(c3));
    te_cost_add(&c3, c2, delta);
    return te_cost_cmp(c1, &c3);
}
int vertex_cmp(void* node1, void* node2)
{
    struct spf_vertex* pot_hop1 = (struct spf_vertex* )node1;
    struct spf_vertex* pot_hop2 = (struct spf_vertex* )node2;

    return te_cost_cmp(&pot_hop1->te_cost, &pot_hop2->te_cost);
}

unsigned int int_hash( int a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3); // <<和 +的组合是可逆的
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

unsigned int vertex_get_hash_key(struct spf_vertex *vertex)
{
    return int_hash(vertex->router->id);
    //return vertex->r->id;

}
static void vertex_hash_clear (struct hash_backet *bucket, void *args)
{
    zebra_hash_release ((struct hash*)args, bucket->data);
}


static void record_hash_print(struct hash_backet *bucket, void *args[])
{
    struct spf_vertex *a = (struct spf_vertex *)bucket->data;
    if (a)
        printf("\r\n vertex:0x%x", (int)a);
    else
        printf("?");
}

void vertex_hash_tbl_print(struct hash *h)
{
    zebra_hash_iterate(h,
                       (void(*)(struct hash_backet*,void*))record_hash_print,
                       NULL);
}

static void vertex_hash_tbl_clear (struct hash *h)
{
    zebra_hash_iterate (h,
                        (void (*) (struct hash_backet*,void*)) vertex_hash_clear,
                        h);
}

static int vertex_hash_cmp(struct spf_vertex *vertex1, struct spf_vertex *vertex2)
{
    return (vertex1->router->id == vertex2->router->id);
}

unsigned int vertex_hash_tbl_destroy(struct hash *h)
{
    if (h)
    {
        zebra_hash_clean (h, NULL);
        zebra_hash_free (h);
        h = NULL;
    }
    return 0;
}

struct spf_vertex * vertex_hash_lookup(struct hash *h, struct spf_vertex * vertex)
{
    struct spf_vertex *hit = zebra_hash_lookup(h, vertex);
    return hit;
}
static void * vertex_hash_alloc (struct spf_vertex *vertex)
{
    return vertex;
}

static void * vertex_hash_add (struct hash *h, struct spf_vertex *vertex)
{
    return zebra_hash_get(h, vertex,
                          (void * (*) (void *))vertex_hash_alloc);
}

static void vertex_hash_del (struct hash * h, struct spf_vertex *vertex)
{
    zebra_hash_release(h, vertex);
}

static struct spf_vertex * find_vertex (struct hash *h, void *r)
{
    struct spf_vertex vertex_tmp;
    struct spf_vertex *vertex_hit = NULL;

    vertex_tmp.router = r;
    vertex_hit = vertex_hash_lookup(h, &vertex_tmp);

    return vertex_hit;

}
static struct spf_vertex * vertex_new (struct spftree_s *spftree, ROUTER *r)
{
    struct spf_vertex *vertex;

    vertex = XMALLOC (MTYPE_VERTEX, sizeof (struct spf_vertex));
    if(NULL == vertex)
    {
        ERR_LOG(0,0,0,0);
        return NULL;
    }

    memset(vertex, 0, sizeof (struct spf_vertex));

    vertex->router = r;
    vertex->spftree = spftree;
    vertex->path_array = zebra_vector_init(1);
    vertex->parents_link = list_new ();
    vertex->parents = list_new ();
    vertex->children = list_new ();
    vertex->circuits = list_new ();
    //AVLL_INIT_TREE(vertex->path_srlgs,
    //                compare_ulong,
    //               NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, srlg),
    //                NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, avll_node));

    return vertex;
}
static void vertex_del (struct spf_vertex *vertex)
{
    if (vertex->path_array)
    {
        free_path_vector(vertex->path_array);
        vertex->path_array = NULL;
    }
    list_delete (vertex->parents_link);
    vertex->parents_link = NULL;
    list_delete (vertex->parents);
    vertex->parents = NULL;
    list_delete (vertex->children);
    vertex->children = NULL;
    list_delete(vertex->circuits);
    vertex->circuits = NULL;

    //path_srlg_tree_free(&vertex->path_srlgs);

    memset(vertex, 0, sizeof(struct spf_vertex));
    XFREE (MTYPE_VERTEX, vertex);

    return;
}

static void spf_vertex_update_stat (void *node , int position)
{
    struct spf_vertex *v = node;

    /* Set the status of the vertex, when its position changes. */
    v->stat = position;
}
int pq_vertex_cmp(void* node1, void* node2)
{
    return vertex_cmp(node1, node2);
}


/**PROC+**********************************************************************/
/* Name:      compare_ulong                                                  */
/*                                                                           */
/* Purpose:   Standard function for comparing ints                     */
/*                                                                           */
/* Returns:   -1 if aa < bb                                                  */
/*             0 if aa = bb                                                  */
/*             1 if aa > bb                                                  */
/*                                                                           */
/* Params:    IN  aa                                                         */
/*            IN  bb                                                         */
/*                                                                           */
/**PROC-**********************************************************************/
int compare_ulong(void *aa, void *bb  )
{
    /***************************************************************************/
    /* Local Variables                                                         */
    /***************************************************************************/
    int ret_val;

    NBB_TRC_ENTRY("compare_ulong");

    if (*(int *)aa < *(int *)bb)
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED    Reason: Performance.                     */
        /*************************************************************************/
        ret_val = -1;
    }
    else if (*(int *)aa > *(int *)bb)
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED    Reason: Performance.                     */
        /*************************************************************************/
        ret_val = 1;
    }
    else
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED    Reason: Performance.                     */
        /*************************************************************************/
        ret_val = 0;
    }

    NBB_TRC_EXIT();

    return(ret_val);

} /* compare_ulong */


QCCS_SRLG_EXCLUDE *qccs_find_link_srlg_in_tree(LINK_RECORD *link_record,
        AVLL_TREE *srlg_tree)
{
    /***************************************************************************/
    /* Local Variables                                                         */
    /***************************************************************************/
    int next_srlg;
    int *srlg_p;
    QCCS_SRLG_EXCLUDE *current_srlg;
    int ii;

    NBB_TRC_ENTRY("qccs_find_link_srlg_in_tree");

    NBB_TRC_DETAIL((NBB_FORMAT "retcode on entry: %bu", (*retcode)));

    if (link_record->num_srlgs != 0)
    {
        /*************************************************************************/
        /* A link may be a member of a number of SRLGs.  We need to confirm that */
        /* none of these are excluded by this query.                             */
        /*************************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "link record %p has SRLG entry %p",
                      link_record, link_record->srlg_entry));

        for (ii = 0; ii < link_record->num_srlgs; ii++)
        {
            next_srlg = link_record->srlg_entry[ii];

            current_srlg = (QCCS_SRLG_EXCLUDE *)AVLL_FIND(*srlg_tree, &next_srlg);
            if (current_srlg != NULL)
            {
                return current_srlg;
            }
        }
    }

EXIT_LABEL:

    NBB_TRC_DETAIL((NBB_FORMAT "retcode now %bu", (*retcode)));
    NBB_TRC_EXIT();

    return NULL;

} /* qccs_chk_link_srlg_constraints */

/**PROC+**********************************************************************/
/* Name:      qccs_chk_link_srlg_constraints                                 */
/*                                                                           */
/* Purpose:   Used to check whether a link satisfies the SRLG constraints as */
/*            specified on a route query.                                    */
/*                                                                           */
/* Returns:   Nothing.                                                       */
/*                                                                           */
/* Params:    IN     link_record  - The link record to check.                */
/*            IN/OUT retcode      - A bit mask representing the following    */
/*                                  values                                   */
/*                                                                           */
/*            -  QCCS_NO_EXCLUDED_RESOURCES if the link does not violate any */
/*               constraints.                                                */
/*            -  QCCS_EXCLUDED_SRLG if the link is a member of an excluded   */
/*               SRLG.                                                       */
/*            -  QCCS_EXCLUDED_NODE if the link leads to an excluded node.   */
/*            -  QCCS_EXCLUDED_LINK if the link is excluded.                 */
/*            -  QCCS_LINK_UNUSABLE if the link has violated an inviolable   */
/*               constraint.                                                 */
/*                                                                           */
/* Operation: Check the link's SRLGs against those in the current query.     */
/*                                                                           */
/**PROC-**********************************************************************/

void qccs_chk_link_srlg_constraints(LINK_RECORD *link_record,
                                    int *retcode
                                   )
{
    /***************************************************************************/
    /* Local Variables                                                         */
    /***************************************************************************/
    int next_srlg;
    int *srlg_p;
    QCCS_SRLG_EXCLUDE *current_srlg;
    int ii;

    NBB_TRC_ENTRY("qccs_chk_link_srlg_constraints");

    NBB_TRC_DETAIL((NBB_FORMAT "retcode on entry: %bu", (*retcode)));

    if (link_record->num_srlgs != 0)
    {
        /*************************************************************************/
        /* A link may be a member of a number of SRLGs.  We need to confirm that */
        /* none of these are excluded by this query.                             */
        /*************************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "link record %p has SRLG entry %p",
                      link_record, link_record->srlg_entry));

        for (ii = 0; ii < link_record->num_srlgs; ii++)
        {
            next_srlg = link_record->srlg_entry[ii];

            current_srlg = (QCCS_SRLG_EXCLUDE *)AVLL_FIND(
                               current_query.primary_srlg_exclusion_tree,
                               &next_srlg);

            if (current_srlg != NULL)
            {
                /*********************************************************************/
                /* This SRLG has been excluded for this query.                       */
                /*********************************************************************/
                NBB_TRC_FLOW((NBB_FORMAT "SRLG %lu is excluded", next_srlg));

                if (current_srlg->exclusion_level & QCCS_ALWAYS_EXCLUDE)
                {
                    NBB_TRC_FLOW((NBB_FORMAT "Always exclude SRLG %lu", next_srlg));
                    (*retcode) = QCCS_LINK_UNUSABLE;
                    goto EXIT_LABEL;
                }

                if (current_srlg->exclusion_level & QCCS_EXCLUDE_IF_POSSIBLE)
                {
                    /*******************************************************************/
                    /* Try and exclude this SRLG.                                      */
                    /*******************************************************************/
                    NBB_TRC_FLOW((NBB_FORMAT "Try and exclude SRLG %lu", next_srlg));
                    (*retcode) = (int)((*retcode) | QCCS_EXCLUDED_SRLG);
                }
            }
        }
    }

EXIT_LABEL:

    NBB_TRC_DETAIL((NBB_FORMAT "retcode now %bu", (*retcode)));
    NBB_TRC_EXIT();

    return;

} /* qccs_chk_link_srlg_constraints */

int check_link_usable(LINK_RECORD *link)
{
    int ret = 0;
    if (link->from->spf_query_num == current_query.g_spftree->query_num)
    {
        if (link->from->exclude == QCCS_ALWAYS_EXCLUDE)
        {
            ret |= QCCS_LINK_UNUSABLE;
            goto out;
        }
        if (link->from->exclude == QCCS_EXCLUDE_IF_POSSIBLE)
        {
            ret |= QCCS_EXCLUDED_NODE;
        }
    }
    if (link->to->spf_query_num == current_query.g_spftree->query_num)
    {
        if (link->to->exclude == QCCS_ALWAYS_EXCLUDE)
        {
            ret |= QCCS_LINK_UNUSABLE;
            goto out;
        }
        if (link->to->exclude == QCCS_EXCLUDE_IF_POSSIBLE)
        {
            ret |= QCCS_EXCLUDED_NODE;
        }
    }

    if (link->query_number == current_query.g_spftree->query_num)
    {
        if (link->exclude_level == QCCS_ALWAYS_EXCLUDE)
        {
            ret |= QCCS_LINK_UNUSABLE;
            goto out;
        }
        if (link->exclude_level == QCCS_EXCLUDE_IF_POSSIBLE)
        {
            ret |= QCCS_EXCLUDED_LINK;
        }
    }
    if (link->bandwidth < current_query.constraints.bandwidth)
    {
        ret |= QCCS_LINK_UNUSABLE;
        goto out;
    }
    //qccs_chk_link_srlg_constraints(link, &ret);
    //if (ret & QCCS_LINK_UNUSABLE)
    //     goto out;

out:

    return ret;
}
struct spftree_s * spftree_new ()
{
    struct spftree_s *tree;

    tree = XCALLOC (MTYPE_SPFTREE, sizeof (struct spftree_s));
    if(NULL == tree)
    {
        ERR_LOG(sizeof (struct spftree_s), 0, 0, 0);
        return NULL;
    }
    memset(tree, 0, sizeof (struct spftree_s));

    tree->tents = pqueue_create ();
    if (tree->tents)
    {
        tree->tents->cmp = pq_vertex_cmp;
        tree->tents->update = spf_vertex_update_stat;
    }
    else
    {
        ERR_LOG(0, 0, 0, 0);
    }

    tree->paths = list_new ();
    tree->paths_hash = zebra_hash_create ((unsigned int (*) (void *))vertex_get_hash_key,
                                          (int (*) (const void *, const void *))vertex_hash_cmp);
    tree->tents_hash = zebra_hash_create ((unsigned int (*) (void *))vertex_get_hash_key,
                                          (int (*) (const void *, const void *))vertex_hash_cmp);

    tree->vertex_list = list_new();


    return tree;
}

void spftree_del (struct spftree_s *spftree)
{

    vertex_hash_tbl_destroy(spftree->paths_hash);
    vertex_hash_tbl_destroy(spftree->tents_hash);

    if (spftree->tents)
    {
        pqueue_delete (spftree->tents);
        spftree->tents = NULL;
    }

    list_delete (spftree->paths);
    spftree->paths = NULL;

    if (spftree->vertex_list)
    {
        spftree->vertex_list->del = (void (*)(void *)) vertex_del;
        list_delete (spftree->vertex_list);
    }

    XFREE (MTYPE_SPFTREE, spftree);

    return;
}

/*
 * Add this IS to the root of SPT
 */

static struct spf_vertex *spf_add_root (AVLL_TREE *root_srlgs, int root_srlg_num, struct spftree_s *spftree)
{
    struct spf_vertex *vertex;

    vertex = vertex_new (spftree, spftree->root_router);
    if (!vertex)
    {
        ERR_LOG(0, 0, 0, 0);
        return NULL;
    }
    if (root_srlgs)
    {
        vertex->te_cost.exclusion_overlap.number_of_common_srlgs = root_srlg_num;
        //srlg_tree_add(&vertex->path_srlgs, root_srlgs);
    }
    listnode_add(spftree->vertex_list, vertex);
    listnode_add (spftree->paths, vertex);
    vertex_hash_add(spftree->paths_hash, vertex);

    return vertex;
}

/*
 * Add a vertex to TENT sorted by cost and by vertextype on tie break situation
 */
static struct spf_vertex *spf_add2tent (struct spftree_s *spftree, struct spf_vertex *parent, LINK_RECORD *link, struct spf_vertex *dummy)
{
    struct spf_vertex *vertex = NULL;
    struct listnode *node = NULL;

    if(NULL == spftree)
    {
        return NULL;
    }

    vertex = dummy;
    if (!vertex)
    {
        ERR_LOG(0, 0, 0, 0);
        return NULL;
    }

    listnode_add(spftree->vertex_list, vertex);

    //memcpy(&vertex->te_cost, &dummy->te_cost, sizeof(QCCS_TE_COST));
    // if (current_query.processing_backup)
    // {
    //    srlg_tree_add(&vertex->path_srlgs, &dummy->path_srlgs);
    // }
    if (parent) {
        //listnode_add (vertex_tmp->parents_link, link);
        //listnode_add (vertex_tmp->parents, parent);
        listnode_add (parent->children, dummy);
    }

    pqueue_enqueue(vertex, spftree->tents);
    vertex_hash_add(spftree->tents_hash, vertex);

    return vertex;
}
SPF_PATH * path_new(struct spf_vertex *pvertex, ROUTER *from, ROUTER *to)
{
    SPF_PATH *path = XCALLOC(MTYPE_PATH, sizeof(SPF_PATH));
    if (!path)
    {
        ERR_LOG(0,0,0,0);
        return NULL;
    }
    path->l_lists = list_new();
    path->vertex = pvertex;

    path->from = from;
    path->to = to;
    AVLL_INIT_TREE(path->srlg_tree,
                   compare_ulong,
                   NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, srlg),
                   NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, avll_node));

    return path;
}

void path_free(SPF_PATH *path)
{
    list_delete(path->l_lists);
    path_srlg_tree_free(&path->srlg_tree);
    XFREE(MTYPE_PATH, path);
}
int add_path_from_parent(struct spftree_s *spf_tree, struct spf_vertex *vertex, 
  struct spf_vertex *parent, LINK_RECORD *link, int new_common_srlgs)
{
    int p_path_i = 0;
    int p_path_num = 0;
    //printf("\r\n add path %d", vertex->r->id);
    if (!parent)
    {
        SPF_PATH *path;
        path = path_new(parent, spf_tree->root_router, vertex->router);

        zebra_vector_set(vertex->path_array, path);
        return 0;
    }
    if (parent->path_array == NULL)
    {
        ERR_LOG(spf_tree->root_router->id, parent->router->id, 0, 0);
        return -1;
    }
    //printf("\r\n parnet %d path  count %d", parent->r->id, zebra_vector_count(parent->r_lists));
    for (p_path_i = 0; p_path_i < parent->path_array->active; p_path_i++)
    {
        SPF_PATH *path;
        SPF_PATH *p_path = zebra_vector_lookup(parent->path_array, p_path_i);
        if (!p_path)
            continue;
        p_path_num++;
        path = path_new(parent, spf_tree->root_router, vertex->router);
        memcpy(&path->te_cost, &vertex->te_cost, sizeof(vertex->te_cost));

        list_add_list(path->l_lists, p_path->l_lists);
        listnode_add(path->l_lists, link);
        if (current_query.processing_backup)
        {
          int new_exclude_srlg_num = 0;
          common_srlg_num_calc(&current_query.primary_srlg_exclusion_tree,
            vertex->spftree->root_srlg_tree,
            link, &path->srlg_tree, &new_exclude_srlg_num);
            path->srlg_num = p_path->srlg_num + new_exclude_srlg_num;

          //printf("\r\n vertex new:%d, this path new:%d", new_common_srlgs, new_exclude_srlg_num);
          if (new_common_srlgs == new_exclude_srlg_num)
          {
            zebra_vector_set(vertex->path_array, path);
          }
          else
          {
            path_free(path);
            continue;
          }          
        }
        else
          zebra_vector_set(vertex->path_array, path);
    }
    if (p_path_num == 0)
    {
        SPF_PATH *path;

        path = path_new(parent, spf_tree->root_router, vertex->router);
        memcpy(&path->te_cost, &vertex->te_cost, sizeof(vertex->te_cost));
        listnode_add(path->l_lists, link);
        //path->te_cost.exclusion_overlap.number_of_common_srlgs = vertex->spftree->root_srlg_num;
        memcpy(&path->te_cost, &vertex->te_cost, sizeof(vertex->te_cost));
        
        if (current_query.processing_backup)
        {
          int new_exclude_srlg_num = 0;
          common_srlg_num_calc(&current_query.primary_srlg_exclusion_tree,
            vertex->spftree->root_srlg_tree,
            link, &path->srlg_tree, &new_exclude_srlg_num);   
                      path->srlg_num = vertex->spftree->root_srlg_num + new_exclude_srlg_num;

          if (new_common_srlgs == new_exclude_srlg_num)
          {
            zebra_vector_set(vertex->path_array, path);
          }
          else
          {
            path_free(path);
          }          
        }
        else
          zebra_vector_set(vertex->path_array, path);
    }
    return 0;
}
#if 0
int del_path_from_parent(struct spf_vertex *vertex, struct spf_vertex *parent)
{
    int path_i = 0;
    for (path_i = 0; path_i < vertex->path_array->active; path_i++)
    {
        SPF_PATH *path = zebra_vector_lookup(vertex->path_array, path_i);
        if (!path)
            continue;
        if (path->parent == parent)
        {
            zebra_vector_unset(vertex->path_array, path_i);
            path_free(path);

        }
    }
    return 0;

}
#endif

int merge_path(vector target, vector add)
{
    int path_i = 0;

    for (path_i = 0; path_i < add->active; path_i++)
    {
        SPF_PATH *path = zebra_vector_lookup(add, path_i);
        if (path)
        {
            zebra_vector_unset(add, path_i);
            zebra_vector_set(target, path);
        }
    }
}
int merge_spf_vertex_ecmp( struct spf_vertex *vertex,
                           struct spf_vertex *parent_new,
                           struct spf_vertex *vertex_new,
                           LINK_RECORD *link)
{
    if (listnode_lookup (vertex->parents, parent_new) == NULL)
    {
        listnode_add (vertex->parents_link, link);
        listnode_add (vertex->parents, parent_new);
        if (vertex->te_cost.number_of_hops < parent_new->te_cost.number_of_hops + 1)
            vertex->te_cost.number_of_hops  = parent_new->te_cost.number_of_hops  + 1;
        //add_path_from_parent(vertex->spftree, vertex, parent_new, link);
        merge_path(vertex->path_array, vertex_new->path_array);
    }

    if (listnode_lookup (parent_new->children, vertex) == NULL)
        listnode_add (parent_new->children, vertex);

}
int replace_spf_vertex( struct spf_vertex *vertex, struct spf_vertex *parent_new, struct spf_vertex *vertex_new,
                        LINK_RECORD *link)
{
    struct listnode *pnode, *pnextnode;
    struct spf_vertex *pvertex;
    struct listnode *node;
    //listnode_delete (spftree->tents, vertex);
    vertex_hash_del(vertex->spftree->tents_hash, vertex);

    if(listcount (vertex->children) != 0)
    {
        ERR_LOG(listcount (vertex->children), 0, 0, 0);
        return 0;
    }

    for (ALL_LIST_ELEMENTS (vertex->parents, pnode, pnextnode, pvertex))
    {
        if(NULL == pvertex)
        {
            continue;
        }
        //del_path_from_parent(vertex, pvertex);
        listnode_delete(pvertex->children, vertex);
        //listnode_delete(vertex->parents, pvertex);
        //listnode_delete(vertex->parents_link, link);
    }
    //for (ALL_LIST_ELEMENTS (vertex->parents_link, pnode, pnextnode, plink))
    //{
    //    if(NULL == plink)
    //     {
    //         continue;
    //     }

    //     listnode_delete(vertex->parents_link, plink);
    // }
    //memcpy(&vertex->te_cost, &vertex_tmp->te_cost, sizeof(QCCS_TE_COST));
    //vertex->path_srlgs = vertex_tmp.path_srlgs;
    //if (current_query.processing_backup)
    //
    //{
    //    srlg_tree_add(&vertex->path_srlgs, &vertex_tmp->path_srlgs);
    //}
    //if (parent) {
    //    listnode_add (vertex->parents, parent);
    //    listnode_add (vertex->parents_link, link);
    //   listnode_add (parent->children, vertex);
    //    add_path_from_parent(spftree, vertex, parent, link);
    // }
    if (parent_new) {
        listnode_add (parent_new->children, vertex_new);
    }

    pqueue_replace_index(vertex->stat, vertex_new,  vertex->spftree->tents);
    // trickle_up (vertex->stat, vertex->spftree->tents);

    vertex_hash_add(vertex->spftree->tents_hash, vertex_new);
    listnode_delete(vertex->spftree->vertex_list, vertex);

    vertex_del(vertex);
    return 0;
}
int best_common_srlg_num_calc(AVLL_TREE *primary_srlg_exclusion_tree,
                              struct spf_vertex *parent,
                              LINK_RECORD *this_link,
                              AVLL_TREE *new_path_srlgs,
                              unsigned int *common_srlg_num_new)
{
    unsigned int best = 0xffffffff ;
    unsigned int new_common_srlgs = 0;
    int p_path_i;
    AVLL_TREE *parent_path_srlgs;
    int p_path_num = 0;
    for (p_path_i = 0; p_path_i < parent->path_array->active; p_path_i++)
    {
        SPF_PATH *path;
        SPF_PATH *p_path = zebra_vector_lookup(parent->path_array, p_path_i);
        
        if (!p_path)
            continue;
        p_path_num++;
        parent_path_srlgs = &p_path->srlg_tree;
        
        AVLL_INIT_TREE(*new_path_srlgs,
               compare_ulong,
               NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, srlg),
               NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, avll_node));

        common_srlg_num_calc(primary_srlg_exclusion_tree,
                             &parent_path_srlgs,
                             this_link, new_path_srlgs, &new_common_srlgs);
        if (best > new_common_srlgs)
            best = new_common_srlgs;
    }

    if (p_path_num == 0)
    {
        common_srlg_num_calc(primary_srlg_exclusion_tree,
                             parent->spftree->root_srlg_tree,
                             this_link, new_path_srlgs, &best);
    }
    *common_srlg_num_new = best;

}

static struct spf_vertex * process_N (struct spftree_s *spftree,
                                      LINK_RECORD *link,
                                      struct spf_vertex *parent)
{
    struct spf_vertex *vertex;
    struct spf_vertex *vertex_tmp;
    LINK_RECORD *plink;
    int ret_code = 0;
    int link_exclude_num = 0;
    int node_exclude_num = 0;
    int srlg_exclude_num = 0;
    AVLL_TREE srlg_tree;
    int new_common_srlgs = 0;
    
    if((NULL == spftree) || (parent == NULL))
    {
        ERR_LOG ((int)spftree, (int)parent, 0, 0);
        return NULL;
    }

    vertex_tmp = vertex_new (spftree, link->to);
    if (!vertex)
    {
        ERR_LOG(0, 0, 0, 0);
        return NULL;
    }


    ret_code = check_link_usable(link);
    if (ret_code & QCCS_LINK_UNUSABLE)
        goto out;
    if (ret_code & QCCS_EXCLUDED_LINK)
    {
        link_exclude_num = parent->te_cost.exclusion_overlap.number_of_common_links + 1;
    }
    if (ret_code & QCCS_EXCLUDED_NODE)
    {
        node_exclude_num = parent->te_cost.exclusion_overlap.number_of_common_routers + 1;
    }
    if (current_query.processing_backup)
    {
        AVLL_INIT_TREE(srlg_tree,
                       compare_ulong,
                       NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, srlg),
                       NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, avll_node));

        best_common_srlg_num_calc(&current_query.primary_srlg_exclusion_tree,
                                  parent,
                                  link, &srlg_tree, &new_common_srlgs);

        srlg_exclude_num = parent->te_cost.exclusion_overlap.number_of_common_srlgs + new_common_srlgs;
        //vertex_tmp.path_srlgs = srlg_tree;
        //srlg_tree_add(vertex_tmp.path_srlgs, &srlg_tree);
    }

    vertex_tmp->router = link->to;
    vertex_tmp->te_cost.exclusion_overlap.number_of_common_links = link_exclude_num;
    vertex_tmp->te_cost.exclusion_overlap.number_of_common_routers = node_exclude_num;
    vertex_tmp->te_cost.exclusion_overlap.number_of_common_srlgs = srlg_exclude_num;
    vertex_tmp->te_cost.node_delay = parent->te_cost.node_delay + link->from->delay;
    vertex_tmp->te_cost.link_delay = parent->te_cost.link_delay + link->delay;
    vertex_tmp->te_cost.total_te_metric_cost = parent->te_cost.total_te_metric_cost + link->weight;
    vertex_tmp->te_cost.number_of_hops = parent->te_cost.number_of_hops + 1;

    listnode_add (vertex_tmp->parents_link, link);
    listnode_add (vertex_tmp->parents, parent);
    add_path_from_parent(spftree, vertex_tmp, parent, link, new_common_srlgs);

    vertex = find_vertex (spftree->paths_hash, link->to);
    if (vertex)
    {
        int cmp = 0;
        cmp = vertex_cmp(vertex_tmp, vertex);
        if(cmp < 0)
        {
            ERR_LOG(vertex->router->id, parent->router->id, 0, 0);
            goto out;
        }
        goto out;
    }

    vertex = find_vertex (spftree->tents_hash, link->to);

    if (vertex)
    {
        int cmp = 0;

        cmp = vertex_cmp(vertex, vertex_tmp);
        if (cmp == 0)
        {
            merge_spf_vertex_ecmp(vertex, parent, vertex_tmp, link);
            goto out;
        }
        else if (cmp < 0)
        {
            goto out;
        }
        else
        {
            replace_spf_vertex(vertex, parent, vertex_tmp, link);
            vertex_tmp = NULL;

            goto out;
        }
    }

    vertex = spf_add2tent (spftree, parent, link, vertex_tmp);
    vertex_tmp = NULL;

out:
    if (vertex_tmp != NULL)
    {
        vertex_del(vertex_tmp);
    }
    if (current_query.processing_backup)
    {
        path_srlg_tree_free(&srlg_tree);
    }
    return vertex;
}

static int spf_process_link (struct spftree_s *spftree, struct spf_vertex *parent)
{
    struct listnode *node = NULL;
    LINK_RECORD *link;

    if (parent->router->must_link)
    {
        process_N(spftree, parent->router->must_link, parent);
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO (parent->router->links, node, link))
        {
            if(NULL == link)
            {
                continue;
            }

            process_N (spftree, link, parent);
        }
    }

    return 0;
}

static int spf_preload_tent (struct spftree_s *spftree,
                             struct spf_vertex *parent)
{
    LINK_RECORD *link;
    struct listnode *node;
    if (parent->router->must_link)
    {
        process_N(spftree, parent->router->must_link, parent);
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO (spftree->root_router->links, node, link))
        {
            if(NULL == link)
                continue;

            process_N (spftree, link, parent);
        }
    }
    return 0;
}
/*
 * The parent(s) for vertex is set when added to TENT list
 * now we just put the child pointer(s) in place
 */
static int add_to_paths (struct spftree_s *spftree, struct spf_vertex *vertex)
{
    int ret = 0;

    if (find_vertex (spftree->paths_hash, vertex->router))
        return ret;

    listnode_add (spftree->paths, vertex);
    vertex_hash_add(spftree->paths_hash, vertex);

    return ret;
}

void init_spt (struct spftree_s *spftree)
{
    if (spftree->paths_hash)
        vertex_hash_tbl_clear(spftree->paths_hash);
    if (spftree->tents_hash)
        vertex_hash_tbl_clear(spftree->tents_hash);

    if (spftree->tents)
        pqueue_delete (spftree->tents);
    spftree->tents = pqueue_create();
    if (spftree->tents)
    {
        spftree->tents->cmp = pq_vertex_cmp;
        spftree->tents->update = spf_vertex_update_stat;
    }

    list_delete_all_node (spftree->paths);
    if (spftree->vertex_list)
    {
        spftree->vertex_list->del = (void (*)(void *)) vertex_del;
        list_delete_all_node(spftree->vertex_list);
    }

    return;
}

char * path_to_string(vector r_lists)
{
    int path_i = 0;
    static char buf[255] = {0};
    int offset = 0;

    for (path_i = 0; path_i < r_lists->active; path_i++)
    {
        struct listnode *node;
        LINK_RECORD *v;

        SPF_PATH *path = zebra_vector_lookup_ensure(r_lists, path_i);
        if (!path)
            continue;
        if (path_i == 0)
            offset += sprintf(buf + offset, "from %d to %d. cost:%d, exclude_link_num:%d, exclude_link_srlg:%d:",
                              path->from->id, path->to->id,
                              path->te_cost.total_te_metric_cost,
                              path->te_cost.exclusion_overlap.number_of_common_links,
                              path->te_cost.exclusion_overlap.number_of_common_srlgs);

        offset += sprintf(buf + offset, "[ ");
        for (ALL_LIST_ELEMENTS_RO (path->l_lists, node, v))
        {
            offset += sprintf(buf + offset, "%d ", v->to->id);
        }
        offset += sprintf(buf + offset, "]");
    }
    return buf;
}
#if 0
vector build_path(struct spf_vertex *vertex )
{
    vector path_array;
    int p_path_i = 0;
    SPF_PATH *path;
    struct spf_vertex *cur_vertex;
    struct fifo pq;
    static int built_num = 0;
    cur_vertex = vertex;
    struct listnode *node;
    struct listnode *pv_node;
    LINK_RECORD *link;
    struct hop_queue *hop_elem = XCALLOC(MTYPE_FIFO, sizeof(struct hop_queue));
    if (!hop_elem)
    {
        ERR_LOG(0,0,0,0);
        return NULL;
    }
    hop_elem->vertex = vertex;
    FIFO_INIT(&pq);

    FIFO_ADD(&pq, hop_elem);
    built_num++;
    while(hop_elem = (struct hop_queue *)FIFO_TOP(&pq))
    {
        cur_vertex = hop_elem->vertex;

        FIFO_DEL(hop_elem);
        XFREE(MTYPE_FIFO, hop_elem);

        if (cur_vertex->path_built && cur_vertex->path_built_num == built_num)
            continue;

        pv_node = listhead(cur_vertex->parents);
        for (ALL_LIST_ELEMENTS_RO(cur_vertex->parents_link, node, link))
        {
            struct spf_vertex *p_vertex;

            if (!link)
                continue;
            p_vertex = listgetdata(pv_node);
            if (!p_vertex)
                continue;

            pv_node = listnextnode(pv_node);

            if (p_vertex->path_built_num != built_num && p_vertex->path_array)
            {
                free_path_vector(p_vertex->path_array);
                p_vertex->path_array = NULL;
            }

            p_vertex->path_built_num = built_num;

            if (p_vertex->path_array == NULL)
                p_vertex->path_array = zebra_vector_init(1);

            hop_elem = XCALLOC(MTYPE_FIFO, sizeof(struct hop_queue));
            if (!hop_elem)
            {
                ERR_LOG(0,0,0,0);
                return NULL;
            }
            hop_elem->vertex = p_vertex;
            FIFO_ADD(&pq, hop_elem);

            if (cur_vertex->path_array->active == 0)
            {
                path = path_new(vertex, vertex->spftree->root_router, vertex->router);
                srlg_tree_add(&path->srlg_tree, &vertex->path_srlgs);
                path->srlg_num = vertex->te_cost.exclusion_overlap.number_of_common_srlgs;
                memcpy(&path->te_cost, &cur_vertex->te_cost, sizeof(QCCS_TE_COST));
                listnode_add(path->l_lists, link);
                zebra_vector_set(p_vertex->path_array, path);
            }
            else
            {
                for (p_path_i = 0; p_path_i < cur_vertex->path_array->active; p_path_i++)
                {
                    SPF_PATH *path;
                    SPF_PATH *p_path = zebra_vector_lookup_ensure(cur_vertex->path_array, p_path_i);
                    if (!p_path)
                        continue;
                    path = path_new(vertex, vertex->spftree->root_router, vertex->router);
                    srlg_tree_add(&path->srlg_tree, &vertex->path_srlgs);
                    path->srlg_num = vertex->te_cost.exclusion_overlap.number_of_common_srlgs;
                    memcpy(&path->te_cost, &p_path->te_cost, sizeof(QCCS_TE_COST));
                    list_add_list(path->l_lists, p_path->l_lists);
                    listnode_add_first(path->l_lists, link);
                    zebra_vector_set(p_vertex->path_array, path);
                }
            }
        }

        cur_vertex->path_built = 1;
        cur_vertex->path_built_num = built_num;

        if (listcount(cur_vertex->parents_link))
        {
            free_path_vector(cur_vertex->path_array);
            cur_vertex->path_array = NULL;
        }
    }
    path_array = vertex->spftree->root_vertex->path_array;
    vertex->spftree->root_vertex->path_array = NULL;
    return  path_array;

}
#endif
int run_spf(ROUTER *source, ROUTER *destination, vector *result)
{
    int retval = 0;
    struct spf_vertex *vertex;
    struct spf_vertex *root_vertex;
    struct spftree_s *spftree = NULL;

    spftree = current_query.g_spftree;
    if(NULL == spftree)
    {
        ERR_LOG ((int)spftree, 0, (int)0, 0);
        return -1;
    }

    spftree->root_router = source;
    init_spt (spftree);
    if (spftree->tents == NULL || spftree->paths == NULL
            || spftree->tents_hash == NULL || spftree->paths_hash == NULL
            || spftree->vertex_list == NULL)
    {
        ERR_LOG (0,0,0, 0);
        goto out;
    }

    root_vertex = spf_add_root (spftree->root_srlg_tree, spftree->root_srlg_num, spftree);
    spftree->root_vertex = root_vertex;
    retval = spf_preload_tent (spftree, root_vertex);
    if (retval != 0)
    {
        ERR_LOG(0,0,0,0);
        goto out;
    }

    if (spftree->tents && spftree->tents->size == 0)
    {
        printf("\r\n-Spf: TENT is empty SPF-root");
        goto out;
    }
    *result = NULL;
    while (spftree->tents && spftree->tents->size > 0)
    {
        /* Remove from tent list and add to paths list */
        vertex = pqueue_dequeue(spftree->tents);
        if(NULL == vertex)
        {
            break;
        }

        vertex_hash_del(spftree->tents_hash, vertex);

        add_to_paths (spftree, vertex);

        if (destination == vertex->router)
        {
            //*result = build_path(vertex);
            *result = vertex->path_array;
            vertex->path_array = NULL;
            break;
        }

        spf_process_link (spftree, vertex);
    }

out:
    return retval;
}
int spf_query_start(ROUTER *source, ROUTER *destination,
                    SPF_CONSTRAINTS *constraints,
                    vector *results)
{
    struct listnode *node;
    ROUTER *r;
    LINK_RECORD *l;
    static int spftree_query_num = 0;
    int ret = 0;
    current_query.g_spftree->query_num = spftree_query_num++;

    //set constraits
    if (constraints)
    {
        for (ALL_LIST_ELEMENTS_RO (constraints->exclude_adjs, node, l))
        {
            l->exclude_level = QCCS_ALWAYS_EXCLUDE;
            l->query_number = current_query.g_spftree->query_num;
        }
        for (ALL_LIST_ELEMENTS_RO (constraints->exclude_if_poss_adjs, node, l))
        {
            int srlg;
            int ii;

            l->exclude_level = QCCS_EXCLUDE_IF_POSSIBLE;
            l->query_number = current_query.g_spftree->query_num;

            l->from->exclude = QCCS_EXCLUDE_IF_POSSIBLE;
            l->from->spf_query_num = current_query.g_spftree->query_num;
            l->to->exclude = QCCS_EXCLUDE_IF_POSSIBLE;
            l->to->spf_query_num = current_query.g_spftree->query_num;


        }
        if (constraints->must_link)
        {
            constraints->must_link->from->must_link = constraints->must_link;
        }
    }
    current_query.g_spftree->root_srlg_num = constraints->root_srlg_num;
    current_query.g_spftree->root_srlg_tree = constraints->root_srlg_tree;
    //run spf until all destination found in path
    run_spf(source, destination, results);

    if (constraints && constraints->must_link)
    {
        constraints->must_link->from->must_link = NULL;
    }

    return 0;
}

