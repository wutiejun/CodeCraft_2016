
#include <base.h>
#include <linklist.h>
#include <memory.h>
#include "memtypes.h"
#include "vector.h"
#include "sdn.h"
#include "file.h"
#include "debug.h"

int dbg = 0;
CURRENT_QUERY_T current_query;

ROUTER* router_new(TOPO *topo, int id)
{
    ROUTER *r = XCALLOC (MTYPE_ROUTER, sizeof (ROUTER));
    if (!r)
        return NULL;
    r->id = id;
    r->links = list_new();
    zebra_vector_set_index(topo->router_list, id, r);
    return r;
}
ROUTER* router_free(TOPO *topo, ROUTER* r)
{
    if (r->links)
    {
        list_delete(r->links);
    }
    if (zebra_vector_lookup_ensure(topo->router_list, r->id))
    {
        zebra_vector_unset(topo->router_list, r->id);
        XFREE(MTYPE_ROUTER, r);
    }
}
ROUTER* router_get(TOPO *topo, int id)
{
    ROUTER *r;
    r = zebra_vector_lookup_ensure(topo->router_list, id);
    if (!r)
    {
        r = router_new(topo, id);
        if (!r)
            return NULL;
    }
    return r;
}
ROUTER* router_lookup(TOPO *topo, int id)
{
    return zebra_vector_lookup_ensure(topo->router_list, id);
}

char *str2intarray(char *cp, char sep_char, int max_cnt, int *array, int *actual_cnt)
{
    register int val = 0, base = 10;
    int cnt = 0;

    /* ignore not digit char */
    while (*cp != '\0' && (*cp < '0' || *cp > '9'))
        cp++;
    
    if (strlen(cp) == 0)
    {
        return cp;
    }

    do
    {
        register char c = *cp;

        if (cnt >= max_cnt)
            break;

        if (c >= '0' && c <= '9')
        {
            val = (val * base) + (c - '0');
        }
        else if (c == sep_char)
        {
            array[cnt] = val;
            cnt++;
            val = 0;
        }
        else if (c == '\0')

        {
            array[cnt] = val;
            cnt++;
            goto out;
        }
    } while (*cp++) ;

out:
    *actual_cnt = cnt;
    return cp;
}
#if 0
int str2intarray(char *cp, vector array, int *num)
{
    register int val = 0, base = 10;
    int cnt = 0;
    int *pint;

    if (strlen(cp) == 0)
    {
        goto out;
    }

    do
    {
        register char c = *cp;

        switch (c)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = (val * base) + (c - '0');
            break;
        case '|':
            cnt++;
            pint = XCALLOC(MTYPE_INT, 4);
            if (!pint)
                goto out;
            *pint = val;
            zebra_vector_set(array, pint);
            val = 0;
            break;
        case '\0':
            cnt++;
            pint = XCALLOC(MTYPE_INT, 4);
            if (!pint)
                goto out;
            *pint = val;
            zebra_vector_set(array, pint);
            goto out;

        default:
            printf("unknow char:%d", c);
            break;
        }
    } while (*cp++) ;

out:
    *num = cnt;
    return 0;
}
#endif
int parse_pass_id_str(char *cp, vector pass_obj_array)
{
    int array[PASS_R_L_MAX] = {0};
    int num = 0;
    int ret = 0;
    int i;

    str2intarray(cp, '|', PASS_R_L_MAX, array, &num);
    for (i = 0; i < num; i+=2)
    {
        int type = array[i+1];
        int id = array[i];
        if (type == PASS_TYPE_ROUTER
                || type == PASS_TYPE_LINK)
        {
            PASS_OBJ *pass_obj = XCALLOC(MTYPE_PASSOBJ, sizeof(PASS_OBJ));
            pass_obj->type = type;
            pass_obj->id = id;
            zebra_vector_set(pass_obj_array, pass_obj);
        }
    }
    return 0;
}
int parse_link_slrg_str(char *cp, int *num, int *srlg)
{
    str2intarray(cp, '|', SRLG_MAX, srlg, num);
}
int parse_node_str(char *cp, NODE_ID *anode)
{
    int array[2] = {0};
    int attr_cnt;
    str2intarray(cp, ',', 2, array, &attr_cnt);
    if (attr_cnt != 2)
    {
        ERR_LOG(attr_cnt,0,0,0);
        return -1;
    }
    anode->id = array[0];
    anode->delay = array[1];
    return 0;
}

int parse_edge_str(char *cp, EDGE *edge)
{
    int array[7] = {0};
    int num;
    char *p = str2intarray(cp, ',', 7, array, &num);
    edge->edge_index = array[EDGE_FIELD_INDEX];
    edge->from_id = array[EDGE_FIELD_FROM];
    edge->to_id = array[EDGE_FIELD_TO];
    edge->peer_id = array[EDGE_FIELD_PEER_ID];
    edge->cost = array[EDGE_FIELD_COST];
    edge->bw = array[EDGE_FIELD_BANDWIDTH];
    edge->delay = array[EDGE_FIELD_DELAY];

    strcpy(edge->srlg_list_str, p);
    return 0;
}
int parse_demand_str(char *cp, SERVICE_DEMAND *primary)
{
    int array[6] = {0};
    int num;
    char *p = str2intarray(cp, ',', 6, array, &num);
    primary->start_id = array[SERVICE_FIELD_START];
    primary->end_id = array[SERVICE_FIELD_END];
    primary->bw = array[SERVICE_FIELD_BANDWIDTH];
    primary->max_delay = array[SERVICE_FIELD_MAX_DELAY];
    primary->max_hop_num = array[SERVICE_FIELD_MAX_HOP];
    parse_pass_id_str(p, primary->pass_obj_array);
}
int parse_service_str(char *cp, QUERY *query)
{
    int array[1] = {0};
    int num;
    char *p = str2intarray(cp, ',', 1, array, &num);

    if (array[SERVICE_FIELD_PATH_TYPE] == 1)
    {
        parse_demand_str(cp, &query->primary);
    }
    else if (array[SERVICE_FIELD_PATH_TYPE] == 2)
    {
        parse_demand_str(cp, &query->backup);
    }

    return 0;
}

LINK_RECORD* link_new(TOPO *topo, EDGE *edge)
{
    LINK_RECORD *l = XCALLOC (MTYPE_LINK_RECORD, sizeof (LINK_RECORD));
    if (!l)
        return NULL;
    l->edge_index = edge->edge_index;
    l->weight = edge->cost;
    l->bandwidth = edge->bw;
    l->delay = edge->delay;
    parse_link_slrg_str(edge->srlg_list_str, &l->num_srlgs, l->srlg_entry);
    zebra_vector_set_index(topo->link_record_list, l->edge_index, l);
    return l;
}
void link_free(TOPO *topo, LINK_RECORD* l)
{
    int i = 0;
    int *pint;
    zebra_vector_unset(topo->link_record_list, l->edge_index);
    XFREE(MTYPE_LINK_RECORD, l);
}
LINK_RECORD * link_lookup(TOPO *topo, int id)
{
    return zebra_vector_lookup_ensure(topo->link_record_list, id);
}

PASS_TREENODE *pass_tree_node_new(ROUTER *router)
{
    PASS_TREENODE *c = XCALLOC(MTYPE_PASS_TREENODE, sizeof(PASS_TREENODE));
    if (!c)
    {
        printf("\r\n NULL");
        return NULL;
    }

    c->parent = NULL;
    c->children = NULL;
    c->cspf_path_array = zebra_vector_init(1);
    c->router = router;
    memset(&c->min_cost, 0, sizeof(c->min_cost));
    listnode_add(current_query.pass_tree_node_list, c);
    router->pass_tnode = c;
    return c;
}
PASS_TREENODE *pass_tree_node_free(PASS_TREENODE *c)
{
    unsigned int i;
    unsigned count = 0;

    if (c->cspf_path_array)
    {
        cspf_path_vector_free(c->cspf_path_array);
    }

    XFREE(MTYPE_PASS_TREENODE, c);
}
PASS_TREENODE * pass_tree_add_children_node(PASS_TREENODE *pnode, PASS_TREENODE *cnode)
{
    cnode->parent = pnode;
    pnode->children = cnode;
    return cnode;
}
PASS_TREENODE * pass_tree_add_children_new_node(PASS_TREENODE *cur_node, ROUTER *node_id)
{
    PASS_TREENODE *new_node = pass_tree_node_new(node_id);
    if (!new_node)
        return NULL;

    //printf("\r\n add child. %d to %d", node_id, cur_node->node_id->id);
    pass_tree_add_children_node(cur_node, new_node);
    return new_node;
}
CSPF_PATH * cspf_path_new()
{
    CSPF_PATH *cpath = XCALLOC(MTYPE_CPATH, sizeof(CSPF_PATH));
    if (!cpath)
    {
        ERR_LOG(0,0,0,0);
        return NULL;
    }
    cpath->l_list = list_new();
    AVLL_INIT_TREE(cpath->srlg_tree,
                   compare_ulong,
                   NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, srlg),
                   NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, avll_node));

    return cpath;
}
int cspf_path_free(CSPF_PATH *cpath)
{
    if (cpath->l_list)
        list_delete(cpath->l_list);
    path_srlg_tree_free(&cpath->srlg_tree);

    XFREE(MTYPE_CPATH, cpath);
}
int cspf_path_vector_free(vector v)
{
    if (v)
    {
        free_vector_all_path(v);
        zebra_vector_free(v);
    }
    return 0;
}
CSPF_PATH * cspf_init_top_path()
{
    CSPF_PATH *cpath = cspf_path_new();
    return cpath;
}
int srlg_tree_add(AVLL_TREE *to, AVLL_TREE *from)
{
    QCCS_SRLG_EXCLUDE *current_srlg;
    if (to == NULL || from == NULL)
        return 0;
    current_srlg = (QCCS_SRLG_EXCLUDE *)AVLL_FIRST(*from);
    while(current_srlg)
    {
        qccs_add_to_srlg_exclusion_tree(current_srlg->srlg,
                                        current_srlg->exclusion_level,
                                        to);
        current_srlg = (QCCS_SRLG_EXCLUDE *)AVLL_NEXT(*from, current_srlg->avll_node);
    }
    return 0;
}
int common_srlg_num_calc(AVLL_TREE *primary_srlg_exclusion_tree,
                         AVLL_TREE *parent_path_srlgs,
                         LINK_RECORD *this_link,
                         AVLL_TREE *new_path_srlgs,
                         int *common_srlg_num_new)
{
    QCCS_SRLG_EXCLUDE *current_srlg;
    QCCS_SRLG_EXCLUDE * hit_srlg;
    int ii;
    int next_srlg;
    int new_srlg_num = 0;

    if (parent_path_srlgs)
        srlg_tree_add(new_path_srlgs, parent_path_srlgs);

    if (this_link->num_srlgs == 0)
    {
        *common_srlg_num_new = 0;
        return 0;
    }

    for (ii = 0; ii < this_link->num_srlgs; ii++)
    {
        next_srlg = this_link->srlg_entry[ii];

        hit_srlg = (QCCS_SRLG_EXCLUDE *)AVLL_FIND(*primary_srlg_exclusion_tree, &next_srlg);
        if (hit_srlg != NULL)
        {
            hit_srlg = qccs_add_to_srlg_exclusion_tree(next_srlg,
                       QCCS_EXCLUDE_IF_POSSIBLE,
                       new_path_srlgs);
            if (hit_srlg)
            {
                continue;
            }

            new_srlg_num++;
        }
    }

    *common_srlg_num_new = new_srlg_num;
    return 0;
}
int cspf_path_update(PASS_TREENODE *pnode, PASS_TREENODE *cnode, SPF_PATH *path)
{
    CSPF_PATH *cpath = cspf_path_new();
    QUERY_CONSTRAINTS *constraints;
    /* 父节点当前选择的路径 add 这一次选择的路径 */
    if (pnode && pnode->cspf_path_array->active)
    {
        CSPF_PATH *parent_cspf_path = zebra_vector_lookup_ensure(pnode->cspf_path_array, pnode->cur_path_index);
        if (parent_cspf_path)
        {
            cpath->te_cost.node_delay += parent_cspf_path->te_cost.node_delay ;
            cpath->te_cost.link_delay += parent_cspf_path->te_cost.link_delay ;
            cpath->te_cost.number_of_hops += parent_cspf_path->te_cost.number_of_hops ;
            cpath->te_cost.total_te_metric_cost += parent_cspf_path->te_cost.total_te_metric_cost ;
            cpath->te_cost.exclusion_overlap.number_of_common_links += parent_cspf_path->te_cost.exclusion_overlap.number_of_common_links ;
            cpath->te_cost.exclusion_overlap.number_of_common_routers += parent_cspf_path->te_cost.exclusion_overlap.number_of_common_routers ;
            //cpath->te_cost.exclusion_overlap.number_of_common_srlgs += parent_cspf_path->te_cost.exclusion_overlap.number_of_common_srlgs ;
            list_add_list(cpath->l_list, parent_cspf_path->l_list);
        }
    }
    cpath->te_cost.node_delay += path->te_cost.node_delay ;
    cpath->te_cost.link_delay += path->te_cost.link_delay ;
    cpath->te_cost.number_of_hops += path->te_cost.number_of_hops ;
    cpath->te_cost.total_te_metric_cost += path->te_cost.total_te_metric_cost ;
    cpath->te_cost.exclusion_overlap.number_of_common_links += path->te_cost.exclusion_overlap.number_of_common_links ;
    cpath->te_cost.exclusion_overlap.number_of_common_routers += path->te_cost.exclusion_overlap.number_of_common_routers ;
    cpath->te_cost.exclusion_overlap.number_of_common_srlgs = path->te_cost.exclusion_overlap.number_of_common_srlgs ;
    cpath->srlg_num = path->srlg_num;
    srlg_tree_add(&cpath->srlg_tree, &path->srlg_tree);
    memcpy(&cnode->min_cost, &cpath->te_cost, sizeof(cnode->min_cost));

    list_add_list(cpath->l_list, path->l_lists);

    zebra_vector_set(cnode->cspf_path_array, cpath);

    if (current_query.processing_backup)
    {
        constraints = &current_query.backup_constraints;
    }
    else
    {
        constraints = &current_query.primary_constraints;
    }

    if (constraints->hop_cnt != 0
            && cpath->te_cost.number_of_hops > constraints->hop_cnt)
    {
        current_query.hop_limit_reached = TRUE;
    }
    if (constraints->delay != 0
            && cpath->te_cost.node_delay + cpath->te_cost.link_delay > constraints->delay)
    {
        current_query.delay_limit_reached = TRUE;
    }
}
int pass_tree_inherit_path(PASS_TREENODE *pnode, PASS_TREENODE *cnode, SPF_PATH *path)
{
    int cmp;
    if (!cnode->calc_path)
    {
        //add adj
        cspf_path_update(pnode, cnode, path);
        cnode->calc_path = TRUE;
    }
    else
    {
        cmp = te_cost_tri_cmp(&cnode->min_cost, &pnode->min_cost, &path->te_cost) ;

        if (cmp < 0 )
        {
            //replace with new adj
            free_vector_all_path(cnode->cspf_path_array);
            cspf_path_update(pnode, cnode, path);

            te_cost_add(&cnode->min_cost, &pnode->min_cost, &path->te_cost);

        }
        else if (cmp == 0)
        {
            //add adj
            cspf_path_update(pnode, cnode, path);
        }
    }

    return 0;

}

int pass_tree_reach_childs(PASS_TREENODE *pnode, SPF_CONSTRAINTS *constraints)
{
    PASS_TREENODE *cnode = pnode->children;
    vector result = NULL;
    int path_i = 0;
    int path_num = 0;

    spf_query_start(pnode->router, cnode->router, constraints, &result);

    if (!result)
        return 0;

    for (path_i = 0; path_i < result->active; path_i++)
    {
        SPF_PATH *path = zebra_vector_lookup_ensure(result, path_i);
        if (!cnode)
            continue;
        if (!path)
            continue;

        if (listcount(path->l_lists))
            path_num++;
        pass_tree_inherit_path(pnode, cnode, path);

    }

    free_path_vector(result);
    if (path_num == 0)
    {
        return RET_ERR_NO_PATH;
    }
    return 0;

}
char * links_to_string_node(struct list *link_lst)
{
    struct listnode *node;
    LINK_RECORD *l;
    static char buf[255] = {0};
    int offset = 0;
    for (ALL_LIST_ELEMENTS_RO (link_lst, node, l))
    {
        if (offset == 0)
            offset += sprintf(buf + offset, ".%d --> %d", l->from->id, l->to->id);
        else
            offset += sprintf(buf + offset, " --> %d", l->to->id);
    }

    return buf;
}
char * links_to_string_link(struct list *link_lst)
{
    struct listnode *node;
    LINK_RECORD *l;
    static char buf[255] = {0};
    int offset = 0;
    int first = TRUE;
    for (ALL_LIST_ELEMENTS_RO (link_lst, node, l))
    {
        if (!first)
            offset += sprintf(buf + offset, "|");
        else
            first = FALSE;

        offset += sprintf(buf + offset, "%d", l->edge_index);
    }

    return buf;
}

int cspf_process_query()
{
    int ret = 0;
    ROUTER *start = current_query.start;
    ROUTER *end = current_query.end;
    SPF_CONSTRAINTS *g_constraints = &current_query.constraints;
    int i = 0;
    PASS_TREENODE *cur;
    PASS_TREENODE *cur_parent;
    struct listnode *sibling_node;
    PASS_TREENODE *child_sibling_tree_node;
    int index;
    QUERY_CONSTRAINTS *query_constaints;
    SPF_CONSTRAINTS spf_constraints;
    int cost = 0;
    int min_cost = 0;
    struct listnode *node;
    ROUTER *router_loop;
    PASS_TREENODE *newc;
    current_query.pass_tree_node_list = list_new();
    if(NULL == current_query.pass_tree_node_list)
    {
        ERR_LOG(0, 0, 0, 0);
        goto EXIT_LABEL;
    }

    current_query.g_pass_tree = XCALLOC(MTYPE_PASS_TREE, sizeof(PASS_TREE));
    if (!current_query.g_pass_tree)
    {
        ERR_LOG(0, 0, 0, 0);
        goto EXIT_LABEL;
    }
    current_query.g_pass_tree->top = pass_tree_node_new(current_query.start);
    if (!current_query.g_pass_tree->top)
    {
        ERR_LOG(0, 0, 0, 0);
        goto EXIT_LABEL;
    }
    current_query.g_pass_tree->bot = pass_tree_node_new(current_query.end);
    if (!current_query.g_pass_tree->bot)
    {
        ERR_LOG(0, 0, 0, 0);
        goto EXIT_LABEL;
    }
    if (current_query.processing_backup)
    {
        query_constaints = &current_query.backup_constraints;
    }
    else
    {
        query_constaints = &current_query.primary_constraints;

    }

    current_query.g_pass_tree->top->next_pass_listnode = listhead(query_constaints->pass_router_list);
    zebra_vector_set(current_query.g_pass_tree->top->cspf_path_array, cspf_init_top_path());
    cur = current_query.g_pass_tree->top;

    i = 0;
    while(cur)
    {
        if (i++>400) break;
        int specified_next_link = FALSE;
        struct link_record *pass_link;

        /* 是否有必经边 */
        if (!current_query.processing_backup
                && cur->router->cspf_query_num == current_query.cspf_query_num
                && cur->router->primary_pass_edge == 1)
        {
            pass_link = cur->router->primary_pass_link;
            specified_next_link = TRUE;
        }
        else if (current_query.processing_backup
                 && cur->router->cspf_query_num == current_query.cspf_query_num
                 && cur->router->backup_pass_edge == 1)
        {
            pass_link = cur->router->backup_pass_link;
            specified_next_link = TRUE;
        }

        if (specified_next_link)
        {
            if (pass_link->to != current_query.end)
            {
                newc = pass_tree_add_children_new_node(cur, pass_link->to);
                if (cur->next_pass_listnode)
                {
                    newc->next_pass_listnode = listnextnode(cur->next_pass_listnode);
                }
            }
            else
            {
                newc = pass_tree_add_children_node(cur, current_query.g_pass_tree->bot);
            }
        }
        else
        {
            int child_num = 0;
            /* 添加下一个必经点为孩子结点 */
            if (cur->next_pass_listnode)
            {
                newc = pass_tree_add_children_new_node(cur, listgetdata(cur->next_pass_listnode));
                newc->next_pass_listnode = listnextnode(cur->next_pass_listnode);
                child_num++;
            }

            if (child_num == 0)
            {
                newc = pass_tree_add_children_node(cur, current_query.g_pass_tree->bot);
            }
        }

        /*  父节点的最小路径可能有多条 */
        for (index = 0; index < vector_active (cur->cspf_path_array); index++)
        {
            CSPF_PATH *cpath = NULL;
            cur->cur_path_index = index;

            memset(&spf_constraints, 0, sizeof(spf_constraints));
            spf_constraints.bandwidth = query_constaints->bandwidth;
            spf_constraints.delay = query_constaints->delay;
            spf_constraints.hop_cnt = query_constaints->hop_cnt;
            if (specified_next_link)
            {
                spf_constraints.must_link = pass_link;
            }
            if (g_constraints && g_constraints->exclude_if_poss_adjs)
            {
                /*  算backup path时，尽量排除primary path的边 */
                spf_constraints.exclude_if_poss_adjs = g_constraints->exclude_if_poss_adjs;
            }

            cpath = zebra_vector_lookup_ensure (cur->cspf_path_array, index);
            if (cpath && cpath->l_list && listcount(cpath->l_list))
            {
                /* 排除父节点走过的边 */
                spf_constraints.exclude_adjs = cpath->l_list;
                spf_constraints.root_srlg_num = cpath->srlg_num;
                spf_constraints.root_srlg_tree = &cpath->srlg_tree;
            }
            ret = pass_tree_reach_childs(cur, &spf_constraints);
            if (current_query.delay_limit_reached && current_query.minimize_delay)
            {
                ret = RET_ERR_NO_PATH;
                goto EXIT_LABEL;
            }
            if (current_query.hop_limit_reached && current_query.minimize_hops)
            {
                ret = RET_ERR_NO_PATH;
                goto EXIT_LABEL;
            }
            if (ret == RET_ERR_NO_PATH)
            {
                goto EXIT_LABEL;
            }
        }

        if (newc == current_query.g_pass_tree->bot)
            break;
        cur = newc;
    }


    vector end_cspf_path_array = current_query.g_pass_tree->bot->cspf_path_array;
    if (!end_cspf_path_array)
        current_query.result.result = RET_ERR_NO_PATH;
    int path_num = zebra_vector_count(end_cspf_path_array);
    if (path_num == 0)
    {
        current_query.result.result = RET_ERR_NO_PATH;
    }
    else
    {
        current_query.result.result = RET_OK;
        current_query.result.path_num = path_num;
        current_query.result.cspf_path_array = end_cspf_path_array;
        current_query.g_pass_tree->bot->cspf_path_array = NULL;
    }

EXIT_LABEL:
    destroy_pass_tree();
    return ret;
}
int destroy_pass_tree()
{
    if (current_query.pass_tree_node_list)
    {
        current_query.pass_tree_node_list->del = (void (*)(void *)) pass_tree_node_free;
        list_delete (current_query.pass_tree_node_list);
    }

    XFREE(MTYPE_PASS_TREE, current_query.g_pass_tree);
}
void node_edge_vector_free(vector v)
{
    int vindex;
    if (!v)
        return;

    for (vindex = 0; vindex < v->active; vindex++)
    {
        EDGE *edge = zebra_vector_lookup_ensure(v, vindex);
        if (edge)
            XFREE(MTYPE_EDGE, edge);
    }
    zebra_vector_free(v);
}

void node_id_vector_free(vector v)
{
    int vindex;
    if (!v)
        return;

    for (vindex = 0; vindex < v->active; vindex++)
    {
        NODE_ID *nodeid = zebra_vector_lookup_ensure(v, vindex);
        if (nodeid)
            XFREE(MTYPE_NODEID, nodeid);
    }
    zebra_vector_free(v);
}
TOPO * load_topo(vector vs, int v_num, vector edges, int edge_num)
{
    int eindex;
    int vindex;
    TOPO *topo = XCALLOC(MTYPE_TOPO, sizeof(TOPO));
    if (!topo)
    {
        return NULL;
    }
    topo->router_list = zebra_vector_init(10);
    topo->link_record_list = zebra_vector_init(10);
    for (vindex = 0; vindex < v_num; vindex++)
    {
        NODE_ID *nodeid = zebra_vector_lookup_ensure(vs, vindex);
        ROUTER *r = router_get(topo, nodeid->id);
        if (!r)
        {
            goto out;
        }
        r->delay = nodeid->delay;
    }
    for (eindex = 0; eindex < edge_num; eindex++)
    {
        ROUTER *from;
        ROUTER *to;
        LINK_RECORD *link;
        EDGE *edge = zebra_vector_lookup_ensure(edges, eindex);

        from = router_get(topo, edge->from_id);
        if (!from)
        {
            goto out;
        }
        to = router_get(topo, edge->to_id);
        if (!to)
        {
            goto out;
        }
        link = link_new(topo, edge);
        if (!link)
        {
            goto out;
        }
        link->from = from;
        link->to = to;
        listnode_add(from->links, link);
    }

out:
    return topo;
}
int unload_topo(TOPO *topo)
{
    int i = 0;

    if (!topo)
        return 0;

    for (i = 0; i < topo->router_list->active; i++)
    {
        ROUTER *r = zebra_vector_lookup_ensure(topo->router_list, i);
        if (!r)
            continue;

        router_free(topo, r);
    }

    for (i = 0; i < topo->link_record_list->active; i++)
    {
        LINK_RECORD *l = zebra_vector_lookup_ensure(topo->link_record_list, i);
        if (!l)
            continue;

        link_free(topo, l);
    }

    zebra_vector_free(topo->router_list);
    zebra_vector_free(topo->link_record_list);
    XFREE(MTYPE_TOPO, topo);
    return 0;
}
int free_vector_all_path(vector v)
{
    int path_i = 0;

    for (path_i = 0; path_i < v->active; path_i++)
    {
        CSPF_PATH *path = zebra_vector_lookup_ensure(v, path_i);
        if (path)
        {
            cspf_path_free(path);
            zebra_vector_unset(v, path_i);
        }
    }
    return 0;
}

int free_path_vector(vector v)
{
    int path_i = 0;

    for (path_i = 0; path_i < v->active; path_i++)
    {
        SPF_PATH *path = zebra_vector_lookup(v, path_i);
        if (path)
        {
            path_free(path);
            zebra_vector_unset(v, path_i);
        }
    }
    zebra_vector_free(v);
}
int path_srlg_tree_free(AVLL_TREE *path_srlgs)
{
    QCCS_SRLG_EXCLUDE *srlg_ptr;
    /***************************************************************************/
    /* Free the SRLG exclusion tree.                                           */
    /***************************************************************************/
    while ((srlg_ptr = (QCCS_SRLG_EXCLUDE *)AVLL_FIRST(
                           *path_srlgs)) != NULL)
    {

        /*************************************************************************/
        /* Remove this element from the tree.                                    */
        /*************************************************************************/
        AVLL_DELETE(*path_srlgs, srlg_ptr->avll_node);

        /*************************************************************************/
        /* Free the element.                                                     */
        /*************************************************************************/
        XFREE(MEM_QCCS_SRLG_EXCLUDE, srlg_ptr);
    }
    return 0;
}

int passobj_to_netobj(TOPO *topo, vector pass_id_array, struct list *pass_router_list, int backup_path)
{
    int i;
    if (pass_id_array && zebra_vector_count(pass_id_array))
    {
        for (i = 0; i < pass_id_array->active; i++)
        {
            ROUTER *pass_router;
            PASS_OBJ *pass_obj = zebra_vector_lookup_ensure(pass_id_array, i);
            if (!pass_obj)
            {
                continue;
            }
            if (pass_obj->type == 0)
            {
                pass_router = router_lookup(topo, pass_obj->id);
                if (!pass_router)
                {
                    ERR_LOG(0, 0, 0, 0);
                    return -1;
                }
                if (listnode_lookup (pass_router_list, pass_router) == NULL)
                    listnode_add(pass_router_list, pass_router);
            }
            else if (pass_obj->type == 1)
            {
                LINK_RECORD *pass_link;

                pass_link = link_lookup(topo, pass_obj->id);
                if (!pass_link)
                {
                    ERR_LOG(0, 0, 0, 0);
                    return -1;
                }

                if (!backup_path)
                {
                    if (pass_link->from->cspf_query_num == current_query.cspf_query_num
                            && pass_link->from->primary_pass_edge == 1
                            && pass_link->from->primary_pass_link->to != pass_link->to)
                        continue;

                    pass_link->from->primary_pass_edge = 1;
                    pass_link->from->primary_pass_link = pass_link;

                }
                else
                {
                    if (pass_link->from->cspf_query_num == current_query.cspf_query_num
                            && pass_link->from->backup_pass_edge == 1
                            && pass_link->from->backup_pass_link->to != pass_link->to)
                        continue;

                    pass_link->from->backup_pass_edge = 1;
                    pass_link->from->backup_pass_link = pass_link;

                }

                pass_link->from->cspf_query_num = current_query.cspf_query_num;

                if (pass_link->from != current_query.start
                        && listnode_lookup (pass_router_list, pass_link->from) == NULL)
                {
                    listnode_add(pass_router_list, pass_link->from);
                }
                if (pass_link->to != current_query.end
                        && listnode_lookup (pass_router_list, pass_link->to) == NULL)
                {
                    listnode_add(pass_router_list, pass_link->to);
                }
            }
            XFREE(MTYPE_PASSOBJ, pass_obj);
            zebra_vector_unset(pass_id_array, i);
        }
    }
    return 0;
}
int cspf_query_parse_constraints(TOPO *topo, QUERY_CONSTRAINTS *qc, SERVICE_DEMAND *demand, int backup)
{
    int ret = 0;
    qc->bandwidth = demand->bw;
    qc->delay = demand->max_delay;
    qc->hop_cnt = demand->max_hop_num;
    qc->pass_router_list = list_new();
    ret = passobj_to_netobj(topo, demand->pass_obj_array, qc->pass_router_list, backup);
    if (ret)
    {
        ERR_LOG(ret, 0, 0, 0);
        return ret;
    }
}
int cspf_query_parse(TOPO *topo, QUERY *query)
{
    int i  = 0;
    int ret = 0;
    static int cspf_query_num = 0;
    memset(&current_query, 0, sizeof(current_query));
    current_query.topo = topo;
    current_query.cspf_query_num = cspf_query_num++;
    current_query.required_backup = TRUE;

    if (query->primary.start_id == query->primary.end_id
            || query->backup.start_id == query->backup.end_id)
    {
        ERR_LOG(query->primary.start_id, query->primary.end_id,
                query->backup.start_id, query->backup.end_id);
        return -1;
    }

    if (            query->primary.start_id != query->backup.start_id
                    || query->primary.end_id != query->backup.end_id)
    {
        ERR_LOG(query->primary.start_id, query->primary.end_id,
                query->backup.start_id, query->backup.end_id);
        current_query.required_backup = FALSE;
    }


    current_query.start = router_lookup(topo, query->primary.start_id);
    if(NULL == current_query.start)
    {
        ERR_LOG(0, 0, 0, 0);
        return -1;
    }

    current_query.end = router_lookup(topo, query->primary.end_id);
    if(NULL == current_query.end)
    {
        ERR_LOG(0, 0, 0, 0);
        return -1;
    }

    cspf_query_parse_constraints(topo, &current_query.primary_constraints, &query->primary, 0);
    cspf_query_parse_constraints(topo, &current_query.backup_constraints, &query->backup, 1);

    current_query.g_spftree = spftree_new();
    if(NULL == current_query.g_spftree)
    {
        ERR_LOG(0, 0, 0, 0);
        return -1;
    }
    AVLL_INIT_TREE(current_query.primary_srlg_exclusion_tree,
                   compare_ulong,
                   NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, srlg),
                   NBB_OFFSETOF(QCCS_SRLG_EXCLUDE, avll_node));
    return 0;
}

int cspf_clear_query()
{

    if (current_query.primary_constraints.pass_router_list)
        list_delete(current_query.primary_constraints.pass_router_list);
    if (current_query.backup_constraints.pass_router_list)
        list_delete(current_query.backup_constraints.pass_router_list);
    if (current_query.g_spftree)
        spftree_del(current_query.g_spftree);


}
char * cspf_path_tostring_link(CSPF_PATH *cpath, int backup)
{
    struct listnode *node;
    LINK_RECORD *l;
    static char buf[255] = {0};
    int offset = 0;
    int first = TRUE;
    offset += sprintf(buf + offset, "%d,", backup?2:1);

    for (ALL_LIST_ELEMENTS_RO (cpath->l_list, node, l))
    {
        if (first)
        {
            offset += sprintf(buf + offset, "%d", l->edge_index);
            first = FALSE;
        }
        else
        {
            offset += sprintf(buf + offset, "|%d", l->edge_index);
        }
    }
    offset += sprintf(buf + offset, "\n");
    return buf;

}
char * cspf_path_tostring(CSPF_PATH *cpath)
{
    static char buf[256] = {0};
    int offset = 0;
    offset += sprintf(buf + offset, "\r\n\t hops:%d nodedelay:%d linkdelay:%d cost:%d \r\n\t exclude srlg num:%d, node num:%d, link num:%d  \r\n\t path:%s\r\n",
                      cpath->te_cost.number_of_hops ,
                      cpath->te_cost.node_delay,
                      cpath->te_cost.link_delay,
                      cpath->te_cost.total_te_metric_cost,
                      cpath->te_cost.exclusion_overlap.number_of_common_srlgs,
                      cpath->te_cost.exclusion_overlap.number_of_common_routers,
                      cpath->te_cost.exclusion_overlap.number_of_common_links,
                      links_to_string_node(cpath->l_list));
    return buf;
}
char * cspf_path_vector_tostring(vector v)
{

    static char buf[256] = {0};
    int offset = 0;
    int index;

    if (!v)
        return buf;

    for (index = 0; index < vector_active (v); index++)
    {
        CSPF_PATH *cpath = zebra_vector_lookup_ensure (v, index);

        if (!cpath)
            continue;
        offset += sprintf(buf + offset, "\r\n\t[%d] hops:%d nodedelay:%d linkdelay:%d cost:%d \r\n\t exclude srlg num:%d, node num:%d, link num:%d  \r\n\t path:%s\r\n",
                          index,
                          cpath->te_cost.number_of_hops ,
                          cpath->te_cost.node_delay,
                          cpath->te_cost.link_delay,
                          cpath->te_cost.total_te_metric_cost,
                          cpath->te_cost.exclusion_overlap.number_of_common_srlgs,
                          cpath->te_cost.exclusion_overlap.number_of_common_routers,
                          cpath->te_cost.exclusion_overlap.number_of_common_links,
                          links_to_string_node(cpath->l_list));
    }
    return buf;
}

char * cspf_path_vector_tostring_link(vector v, int backup)
{

    static char buf[256] = {0};
    int offset = 0;
    int index;

    if (!v)
        return buf;

    for (index = 0; index < vector_active (v); index++)
    {
        CSPF_PATH *cpath = zebra_vector_lookup_ensure (v, index);

        if (!cpath)
            continue;
        offset += sprintf(buf + offset, "%d, %s",
                          backup ? 2 : 1 ,
                          links_to_string_link(cpath->l_list));
        offset += sprintf(buf + offset, "\n");
    }
    return buf;
}


/**PROC+**********************************************************************/
/* Name:      qccs_add_to_srlg_exclusion_tree                                */
/*                                                                           */
/* Purpose:   This procedure is called from qccs_exclude_resources to        */
/*            exclude an SRLG.  It adds the SRLG to the exclusion tree.      */
/*                                                                           */
/* Returns:   TRUE if the resource was successfully added to the tree.       */
/*            FALSE otherwise.                                               */
/*                                                                           */
/* Params:    IN     srlg         - The SRLG to exclude.                     */
/*            IN     exclusion_level                                         */
/*                                - The level of exclusion for this SRLG     */
/*                                                                           */
/* Operation: Allocates the memory for a new exclusion node, fills in the    */
/*            node and inserts it into the tree.                             */
/*                                                                           */
/**PROC-**********************************************************************/

QCCS_SRLG_EXCLUDE * qccs_add_to_srlg_exclusion_tree(int srlg, int exclusion_level, AVLL_TREE *tree)
{
    /***************************************************************************/
    /* Local Variables                                                         */
    /***************************************************************************/
    QCCS_SRLG_EXCLUDE *new_srlg;
    QCCS_SRLG_EXCLUDE *orig_srlg = NULL;
    int success = TRUE;

    NBB_TRC_ENTRY("qccs_add_to_srlg_exclusion_tree");

    /***************************************************************************/
    /* Allocate memory for a new QCCS_SRLG_EXCLUDE structure.                  */
    /***************************************************************************/
    new_srlg = (QCCS_SRLG_EXCLUDE *)XCALLOC(MEM_QCCS_SRLG_EXCLUDE, sizeof(QCCS_SRLG_EXCLUDE));

    if (new_srlg == NULL)
    {
        success = FALSE;
        goto EXIT_LABEL;
    }

    /***************************************************************************/
    /* Initialize this node.                                                   */
    /***************************************************************************/
    AVLL_INIT_NODE(new_srlg->avll_node);

    /***************************************************************************/
    /* Set the SRLG number on this structure.                                  */
    /***************************************************************************/
    new_srlg->srlg = srlg;

    /***************************************************************************/
    /* Set the exclusion type for this structure.                              */
    /***************************************************************************/
    new_srlg->exclusion_level = exclusion_level;

    /***************************************************************************/
    /* Call AVLL_INSERT_OR_FIND to insert into the tree.  (Expect the insert   */
    /* to work, but there are two possibilities for it being there.  One -     */
    /* that the primary route will have used this SRLG despite the exclusion,  */
    /* and that when we come to mark the primary routes resources as now       */
    /* excluded, this resource will already be there.  Two - that the user has */
    /* passed in two identical SRLGs.                                          */
    /***************************************************************************/
    orig_srlg = (QCCS_SRLG_EXCLUDE *)AVLL_INSERT_OR_FIND(
                    *tree,
                    new_srlg->avll_node);
    if (orig_srlg != NULL)
    {
        /*************************************************************************/
        /* SRLG is already in tree.  This is ok, but we should check the         */
        /* exclusion type for this node doesn't need altering.                   */
        /*************************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "Found SRLG (%lu) in tree already.",
                      orig_srlg->srlg));

        /*************************************************************************/
        /* Update the exclusion type for this SRLG.                              */
        /*************************************************************************/
        orig_srlg->exclusion_level |= new_srlg->exclusion_level;

        /*************************************************************************/
        /* Free off our new structure.                                           */
        /*************************************************************************/
        XFREE(MEM_QCCS_SRLG_EXCLUDE, new_srlg);
    }
    else
    {
        NBB_TRC_FLOW((NBB_FORMAT "Added new SRLG (%lu) to tree.",
                      new_srlg->srlg));
    }

EXIT_LABEL:

    NBB_TRC_DETAIL((NBB_FORMAT "Returning return code %B", success));
    NBB_TRC_EXIT();

    return(orig_srlg);

} /* qccs_add_to_srlg_exclusion_tree */

int cspf_query_start(TOPO *topo, QUERY *query, char *outputname)
{
    int ret = 0;
    int index;
    char *primary_out;
    char *backup_out;
    char *primary_out_file;
    char *backup_out_file;
    OS_FILE *output_file = NULL;
    QCCS_SRLG_EXCLUDE *srlg_ptr;
    int ii;
    int primary_found = FALSE;
    int backup_found = FALSE;
    const char *primary_na_str = "1,NA\n";
    const char *backupna_str = "2,NA\n";
    CSPF_PATH *best_primary_path = NULL;
    CSPF_PATH *best_backup_path = NULL;
    int query_again = FALSE;

    if (outputname)
        output_file = OS_FOPEN(outputname, "w");

    ret = cspf_query_parse(topo, query);
    if (ret)
    {
        printf("\r\n query parse error return %d.", ret);
        goto EXIT_LABEL;
    }

    printf("\r\n begin to find primary path...");

    /* query primary path */
    ret = cspf_process_query();
    if (ret)
    {
        printf("\r\n query error return %d.", ret);
        goto EXIT_LABEL;
    }

    if (current_query.delay_limit_reached)
    {
        printf("\r\n primary delay limit reached. try to minimize delay.");

        current_query.delay_limit_reached = FALSE;
        current_query.minimize_delay = TRUE;
        query_again = TRUE;

    }
    if (current_query.hop_limit_reached)
    {
        printf("\r\n primary  hop limit reached. try to minimize hops.");

        current_query.hop_limit_reached = FALSE;
        current_query.minimize_hops = TRUE;
        query_again = TRUE;

    }

    if (query_again)
    {
        ret = cspf_process_query();
    }
    current_query.minimize_delay = FALSE;
    current_query.minimize_hops = FALSE;
    if (ret)
    {
        printf("\r\n query error return %d.", ret);
        goto EXIT_LABEL;
    }
    printf("\r\n primary query result:%d, pathnum:%d, vector addr:%#x",
           current_query.result.result, current_query.result.path_num, current_query.result.cspf_path_array);

    if (current_query.result.cspf_path_array == NULL
            || current_query.result.result != RET_OK
            || current_query.result.path_num == 0)
    {
        printf("\r\n primary path not found.");
        goto EXIT_LABEL;

    }
    primary_found = TRUE;

    printf("\r\n begin to find backup path...");

    /* for every primary path(ecmp) , query backup path */
    current_query.processing_backup = TRUE;

    if (current_query.result.cspf_path_array
            && current_query.result.result == RET_OK
            && current_query.result.path_num)
    {
        int index;
        int index_backup;
        vector primary_path_array = current_query.result.cspf_path_array;
        memset(&current_query.result, 0, sizeof(current_query.result));


        for (index = 0; index < vector_active (primary_path_array); index++)
        {
            struct listnode *node;
            struct listnode *backup_node;
            LINK_RECORD *l;
            LINK_RECORD *backup_link;
            CSPF_PATH *backup_cpath;
            CSPF_PATH *primary_cpath = zebra_vector_lookup_ensure(primary_path_array, index);
            if (!primary_cpath)
                continue;
            if (!best_primary_path)
                best_primary_path = primary_cpath;

            primary_out = cspf_path_tostring(primary_cpath);
            printf("\r\n primary path [%d]: %s", index, primary_out);
            if (current_query.required_backup == FALSE)
            {
                break;
            }

            /*********************************************************************/
            /* Get a pointer to the first SRLG for this link.                    */
            /*********************************************************************/
            for (ALL_LIST_ELEMENTS_RO (primary_cpath->l_list, node, l))
            {
                NBB_TRC_FLOW((NBB_FORMAT "SRLG entry %p for this link", l->srlg_entry));

                for (ii = 0; ii < l->num_srlgs; ii++)
                {
                    NBB_TRC_FLOW((NBB_FORMAT "Adding next SRLG to exclusion list."));
                    qccs_add_to_srlg_exclusion_tree(l->srlg_entry[ii],
                                                    QCCS_EXCLUDE_IF_POSSIBLE,
                                                    &current_query.primary_srlg_exclusion_tree);
                }
            }
            current_query.constraints.exclude_adjs = 0;
            current_query.constraints.exclude_if_poss_adjs = primary_cpath->l_list;
            //primary_cpath->l_lists = NULL;
            /* query backup path */
            cspf_process_query();

            if (current_query.delay_limit_reached)
            {
                printf("\r\n backup delay limit reached. try to minimize delay.");

                current_query.delay_limit_reached = FALSE;
                current_query.minimize_delay = TRUE;
                query_again = TRUE;

            }
            if (current_query.hop_limit_reached)
            {
                printf("\r\n backup  hop limit reached. try to minimize hops.");

                current_query.hop_limit_reached = FALSE;
                current_query.minimize_hops = TRUE;
                query_again = TRUE;

            }

            if (query_again)
            {
                cspf_path_vector_free(current_query.result.cspf_path_array);
                ret = cspf_process_query();
            }
            current_query.minimize_delay = FALSE;
            current_query.minimize_hops = FALSE;
            if (ret)
            {
                printf("\r\n query error return %d.", ret);
                continue;
            }

            if (current_query.result.cspf_path_array == NULL
                    || current_query.result.result != RET_OK
                    || current_query.result.path_num == 0)
            {
                printf("\r\n backup path not found.");
                continue;
            }
            printf("\r\n %d backup path found:", current_query.result.path_num);

            backup_out = cspf_path_vector_tostring(current_query.result.cspf_path_array);
            printf("%s", backup_out);

            for (index_backup = 0; index_backup < vector_active (current_query.result.cspf_path_array); index_backup++)
            {
                backup_cpath = zebra_vector_lookup_ensure(current_query.result.cspf_path_array, index_backup);
                if (!backup_cpath)
                    continue;

                if (best_backup_path == NULL
                        || te_cost_cmp(best_backup_path, backup_cpath) > 0)
                {
                    if (best_backup_path)
                        cspf_path_free(best_backup_path);
                    printf("\r\n selected as best backup path");
                    best_primary_path = primary_cpath;
                    best_backup_path = backup_cpath;
                    zebra_vector_unset(current_query.result.cspf_path_array, index_backup);
                }
            }
            cspf_path_vector_free(current_query.result.cspf_path_array);

            path_srlg_tree_free(&current_query.primary_srlg_exclusion_tree);
            backup_found = TRUE;
            //break;
        }

        if (best_primary_path)
        {
            primary_out_file = cspf_path_tostring_link(best_primary_path, 0);
            if (output_file)
            {
                OS_FWRITE((const char *)primary_out_file, 1, strlen(primary_out_file), output_file);
            }
        }
        if (best_backup_path)
        {
            backup_out_file = cspf_path_tostring_link(best_backup_path, 1);
            if (output_file)
            {
                OS_FWRITE((const char *)backup_out_file, 1, strlen(backup_out_file), output_file);
            }
            cspf_path_free(best_backup_path);
        }

        //free vector
        cspf_path_vector_free(primary_path_array);
    }
    current_query.processing_backup = FALSE;

EXIT_LABEL:
    if (primary_found == FALSE && output_file)
    {
        OS_FWRITE(primary_na_str, 1, strlen(primary_na_str), output_file);

    }
    if (current_query.required_backup && backup_found == FALSE && output_file)
    {
        OS_FWRITE(backupna_str, 1, strlen(backupna_str), output_file);

    }
    if (output_file)
        OS_FCLOSE(output_file);
    cspf_clear_query();
    printf("\r\n query completed.\r\n");
    return ret;
}
int test_query1_1(TOPO *topo)
{
    int ret = 0;
    QUERY query = {0};
    char primary_pass_ids[] = "4|0|5|1";
    char backup_pass_ids[] = "1|1|3|1|7|1";
    int i;
    int type;
    int id;
    query.primary.start_id = 1;
    query.primary.end_id = 7;
    query.primary.pass_obj_array = zebra_vector_init(10);
    query.backup.pass_obj_array = zebra_vector_init(10);

    parse_pass_id_str(primary_pass_ids, query.primary.pass_obj_array);
    parse_pass_id_str(backup_pass_ids, query.backup.pass_obj_array);

    cspf_query_start(topo, &query, NULL);

    zebra_vector_free(query.primary.pass_obj_array);
    zebra_vector_free(query.backup.pass_obj_array);
    return 0;
}
int test_query1_2(TOPO *topo)
{
    int ret = 0;
    QUERY query = {0};
    int pass_ids[] = {2};
    int i;
    query.primary.start_id = 1;
    query.primary.end_id = 7;
    query.primary.pass_obj_array = zebra_vector_init(1);
    for (i = 0; i < array_size(pass_ids); i++)
    {
        //zebra_vector_set(query.pass_id_array, &pass_ids[i]);
    }

    cspf_query_start(topo, &query, NULL);
    zebra_vector_free(query.primary.pass_obj_array);

    return 0;

}
int test_query1_3(TOPO *topo)
{
    int ret = 0;
    QUERY query = {0};
    int i;
    query.primary.start_id = 1;
    query.primary.end_id = 7;
    query.primary.max_hop_num = 5;
    query.primary.pass_obj_array = NULL;

    cspf_query_start(topo, &query, NULL);

    return 0;

}
int test_query2_1(TOPO *topo)
{
    int ret = 0;
    QUERY query = {0};
    char primary_pass_ids[] = "4|0|8|0";
    int i;
    query.primary.start_id = 1;
    query.primary.end_id = 11;
    query.primary.bw = 500;
    query.primary.pass_obj_array = zebra_vector_init(1);

    parse_pass_id_str(primary_pass_ids, query.primary.pass_obj_array);

    cspf_query_start(topo, &query, NULL);
    zebra_vector_free(query.primary.pass_obj_array);
    return 0;

}
int test1()
{
    int ret = 0;
    TOPO *topo = NULL;
    NODE_ID nodes[] = {
        {1, 10},
        {2, 10},
        {3, 10},
        {4, 10},
        {5, 10},
        {6, 10},
        {7, 10},
        {8, 10},

    };
    EDGE edges[] = {
        {0, 1, 2, 1, 1, 1000,10, "1|2"},
        {1, 1, 3, 1, 1, 1000,10, ""},
        {2, 2, 4, 1, 1, 1000,10, "1"},
        {3, 3, 4, 1, 1, 1000,10, "2"},
        {4, 4, 5, 1, 1, 1000,10, ""},
        {5, 4, 6, 1, 1, 1000,10, ""},
        {6, 5, 7, 1, 1, 1000,10, ""},
        {7, 6, 7, 1, 1, 1000,10, ""},
        {8, 1, 4, 1, 1, 1000,10, "1|2|3"},
        //{10, 2, 3, 1},
    };

    vector nodes_vector = zebra_vector_init(10);
    vector edges_vector = zebra_vector_init(10);
    int vindex = 0;
    int eindex = 0;

    for (vindex = 0; vindex < array_size(nodes); vindex++)
    {
        zebra_vector_set(nodes_vector, &nodes[vindex]);
    }
    for (eindex = 0; eindex < array_size(edges); eindex++)
    {
        zebra_vector_set(edges_vector, &edges[eindex]);
    }

    topo  = load_topo(nodes_vector, array_size(nodes), edges_vector, array_size(edges));
    if (!topo)
    {
        printf("\r\n read topo error.");
        return -1;
    }
    zebra_vector_free(nodes_vector);
    zebra_vector_free(edges_vector);

    printf("\r\n -------------begin to test 1 query 1---------------------");
    ret = test_query1_1(topo);
    if (ret)
        return ret;

    printf("\r\n -------------begin to test 1 query 2---------------------");
    //ret = test_query1_2(topo);
    if (ret)
        return ret;

    //printf("\r\n -------------begin to test 1 query 3---------------------");
    ret = test_query1_3(topo);
    if (ret)
        return ret;
    unload_topo(topo);
    return 0;
}

int test2()
{
    int ret = 0;
    TOPO *topo = NULL;
    NODE_ID nodes[] = {
        {1, 10},
        {2, 5},
        {3, 10},
        {4, 10},
        {5, 20},
        {6, 5},
        {7, 10},
        {8, 10},
        {9, 10},
        {10, 10},
        {11, 10},
    };
    EDGE edges[] = {
        {1, 1, 2, 1,1,1000,10, "1|2"},
        {2, 1, 3, 1,1,500, 10,"3"},
        {3, 2, 4, 1,1,500, 10,"1"},
        {4, 3, 5, 1,1,500, 10,"2"},
        {5, 4, 5, 1,1,1000,10, ""},
        {6, 4, 8, 1,1,1000,10, ""},
        {7, 3, 7, 1,1,100, 10,""},
        {8, 5, 6, 1,1,500, 10,""},
        {9, 8, 9, 1,1,500, 10,"1|2|3"},
        {10, 9, 10, 1,1, 600, 10,""},
        {11, 6, 10, 1,1, 10000, 10, ""},
        {12, 10, 11, 1,1, 1000, 10,""},
        {13, 8, 5, 1,1, 500,10, ""},
        {14, 9, 11, 1,1, 300, 10,""},
    };

    vector nodes_vector = zebra_vector_init(10);
    vector edges_vector = zebra_vector_init(10);
    int vindex = 0;
    int eindex = 0;

    for (vindex = 0; vindex < array_size(nodes); vindex++)
    {
        zebra_vector_set(nodes_vector, &nodes[vindex]);
    }
    for (eindex = 0; eindex < array_size(edges); eindex++)
    {
        zebra_vector_set(edges_vector, &edges[eindex]);
    }

    topo  = load_topo(nodes_vector, array_size(nodes), edges_vector, array_size(edges));
    if (!topo)
    {
        printf("\r\n read topo error.");
        return -1;
    }
    zebra_vector_free(nodes_vector);
    zebra_vector_free(edges_vector);
    printf("\r\n -------------begin to test 2 query 1---------------------");
    ret = test_query2_1(topo);
    if (ret)
        return ret;

    unload_topo(topo);
    return 0;
}

int str_blank(char *cp)
{
    while (*cp == ' ' || *cp == '\t'|| *cp == '\n')
        cp++;

    if (strlen(cp) == 0)
        return 1;
    else
        return 0;
}
#if 1

#define MAXLINE     768              /* size of buf for line from .ini file  */
#define MAXFILENAME 256              /* size of buf for file name            */
#define ONETIME_NODE_CNT 100

int read_and_query(char *node_file, char *link_file, char *service_file, char *output_file)
{
    TOPO *topo = NULL;
    OS_FILE *node_file_handle = NULL;
    OS_FILE *link_file_handle = NULL;
    OS_FILE *service_file_handle = NULL;

    unsigned char line_buf[MAXLINE] = {0};     /* buffer to hold       */
    int ret = 0;
    QUERY query = {0};
    int i;
    int type;
    int id;

    vector nodes_vector = zebra_vector_init(10);
    vector edges_vector = zebra_vector_init(10);

    printf("\r\n in: %s %s %s out: %s", node_file, link_file, service_file, output_file);
    
    query.primary.pass_obj_array = zebra_vector_init(10);
    query.backup.pass_obj_array = zebra_vector_init(10);

    node_file_handle = OS_FOPEN(node_file, "r");
    if (node_file_handle == NULL)
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED    Reason: No tracing in this function.     */
        /*************************************************************************/
        goto EXIT;
    }

    /***************************************************************************/
    /* process a line at a time                                                */
    /***************************************************************************/
    while (OS_FGETS(line_buf, MAXLINE, node_file_handle) != NULL)
    {
        NODE_ID *anode;
        if (str_blank(line_buf)) continue;
        anode = XCALLOC(MTYPE_NODEID, sizeof(NODE_ID));
        if (!anode)
            goto EXIT;
        ret = parse_node_str(line_buf, anode);
        if (ret)
            goto EXIT;
        zebra_vector_set(nodes_vector, anode);
    }

    link_file_handle = OS_FOPEN(link_file, "r");
    if (link_file_handle == NULL)
    {
        goto EXIT;
    }
    while (OS_FGETS(line_buf, MAXLINE, link_file_handle) != NULL)
    {
        EDGE *aedge;
        if (str_blank(line_buf)) continue;
        aedge = XCALLOC(MTYPE_EDGE, sizeof(EDGE));        
        if (!aedge)
            goto EXIT;
        parse_edge_str(line_buf, aedge);
        zebra_vector_set(edges_vector, aedge);
    }
    service_file_handle = OS_FOPEN(service_file, "r");
    if (service_file_handle == NULL)
    {
        goto EXIT;
    }
    while (OS_FGETS(line_buf, MAXLINE, service_file_handle) != NULL)
    {
        if (str_blank(line_buf)) continue;
        parse_service_str(line_buf, &query);
    }
    topo  = load_topo(nodes_vector, nodes_vector->active, edges_vector, edges_vector->active);

    /* 查询条件 */
    cspf_query_start(topo, &query, output_file);



EXIT:

    /***************************************************************************/
    if (node_file_handle != NULL)
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED        Reason: No tracing in this function. */
        /*************************************************************************/
        OS_FCLOSE(node_file_handle);
    }
    if (link_file_handle != NULL)
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED        Reason: No tracing in this function. */
        /*************************************************************************/
        OS_FCLOSE(link_file_handle);
    }
    if (service_file_handle != NULL)
    {
        /*************************************************************************/
        /* FLOW TRACING NOT REQUIRED        Reason: No tracing in this function. */
        /*************************************************************************/
        OS_FCLOSE(service_file_handle);
    }

    if (nodes_vector)
        node_id_vector_free(nodes_vector);

    if (edges_vector)
        node_edge_vector_free(edges_vector);

    zebra_vector_free(query.primary.pass_obj_array);
    zebra_vector_free(query.backup.pass_obj_array);

    unload_topo(topo);

}
#endif

int test3()
{
    read_and_query("test3/node.txt", "test3/edge.txt", "test3/service.txt", "test3/output.txt");
    read_and_query("test1/node.txt", "test1/edge.txt", "test1/service.txt", "test1/output.txt");
    read_and_query("test1/node.txt", "test1/edge.txt", "test1/service2.txt", "test1/output2.txt");
    read_and_query("test1/node.txt", "test1/edge.txt", "test1/service3.txt", "test1/output3.txt");
    read_and_query("test1/node.txt", "test1/edge.txt", "test1/service4.txt", "test1/output4.txt");
    read_and_query("test1/node.txt", "test1/edge.txt", "test1/service5.txt", "test1/output5.txt");
    read_and_query("test1/node.txt", "test1/edge6.txt", "test1/service6.txt", "test1/output6.txt");
    read_and_query("test2/node.txt", "test2/edge.txt", "test2/service.txt", "test2/output.txt");
}

int main(int argc, char *argv[])
{
//    show_alloc_memory();
    if (argc == 5)
    {
        /* 从输入的文件里读取参数 */
        read_and_query(argv[1], argv[2], argv[3], argv[4]);
    }
    else
    {
        test3();
    }
    show_alloc_memory();
    return 0;
}

