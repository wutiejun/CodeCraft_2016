
typedef struct atg_qcte_overlap
{
  /***************************************************************************/
  /* The number of routers which are in common on the route.  When reporting */
  /* backup overlap, this field is only relevant when a maximally diverse    */
  /* backup route avoiding common routers has been requested on the query,   */
  /* and returned on the response.  When reporting exclusion overlap, this   */
  /* field gives the number of routers marked for exclusion that have been   */
  /* used on the route.                                                      */
  /***************************************************************************/
  unsigned int number_of_common_routers;

  /***************************************************************************/
  /* The number of links which are in common on the route.  When reporting   */
  /* backup overlap, this field is only relevant when a maximally diverse    */
  /* backup route avoiding common links has been requested on the query, and */
  /* returned on the response.  When reporting exclusion overlap, this field */
  /* gives the number of links marked for exclusion that have been used on   */
  /* the route.                                                              */
  /***************************************************************************/
  unsigned int number_of_common_links;

  /***************************************************************************/
  /* The number of SRLGs which are in common on the route.  When reporting   */
  /* backup overlap, this field is only relevant when a maximally diverse    */
  /* backup route avoiding common SRLGS has been requested on the query, and */
  /* returned on the response.  When reporting exclusion overlap, this field */
  /* gives the number of SRLGs marked for exclusion that have been used on   */
  /* the route.                                                              */
  /***************************************************************************/
  unsigned int number_of_common_srlgs;

} ATG_QCTE_OVERLAP;

typedef struct qccs_te_cost
{
  /***************************************************************************/
  /* The total number of hops from the source to this node.                  */
  /***************************************************************************/
  unsigned int number_of_hops;

  unsigned int node_delay;
  unsigned int link_delay;
  /***************************************************************************/
  /* The total  te metric so far if this hop was taken.                      */
  /***************************************************************************/
  unsigned int total_te_metric_cost;

  /***************************************************************************/
  /* Number of excluded resources included in the route.  These resources    */
  /* comprise of nodes, links and SRLGs.                                     */
  /***************************************************************************/
  ATG_QCTE_OVERLAP exclusion_overlap;

  /***************************************************************************/
  /* Number of resources overlapping between the backup route and the        */
  /* primary route.  Only valid on a backup route calculation.               */
  /***************************************************************************/
  //ATG_QCTE_OVERLAP primary_overlap;
  int path_srlg_num;
  int *path_srlgs;

} QCCS_TE_COST;

typedef struct spf_path_s
{
    struct router *from;
    struct router *to;
    QCCS_TE_COST te_cost;
    struct spf_vertex *vertex;
    struct list *l_lists;
    int srlg_num;
    AVLL_TREE srlg_tree;

}SPF_PATH;

typedef struct cspf_path_s
{
    int cost;
    QCCS_TE_COST te_cost;
    struct list *l_list;
    int srlg_num;
    AVLL_TREE srlg_tree;

}CSPF_PATH;


/*
 * Triple <N, d(N), {Adj(N)}>
 */
struct spf_vertex
{   
    struct router *router;
    struct spftree_s * spftree;
    QCCS_TE_COST te_cost;
    //AVLL_TREE path_srlgs;
    int path_built_num;
    int path_built; /* true or false */
    vector path_array;        /* vector element: vector */
    struct list *parents_link;         
    struct list *parents;         /* list of parents for ECMP */
    struct list *children;        /* list of children used for spftree dump */
    struct list *circuits;  /* only direct route use this field */    
    int stat;
};

//#define SPF_TIME_DEBUG
struct spftree_s
{
    int query_num;
    struct router *root_router;
    struct spf_vertex *root_vertex;
    struct list *paths;             /* the SPT */
    struct pqueue *tents;           /* TENT */
    struct hash *paths_hash;
    struct hash *tents_hash;
    int pending;                    /* already scheduled */
    unsigned int runcount;          /* number of runs since uptime */

    unsigned long long run_starttime;
    int attach;
    struct list *vertex_list;
    int minimize_hops;
    int root_srlg_num;
    AVLL_TREE *root_srlg_tree;
      
#ifdef SPF_TIME_DEBUG
    int time_debug_begin_us;
    int time_debug_while_begin_us;
    int time_debug_addpath_sum_time_us;
    int time_debug_pselsp_sum_time_us;
    int time_debug_lsp_sum_time_us;
    int time_debug_end_us;

    int time_debug_lsp_cnt;
    int time_debug_lsp_sum;
    int time_debug_lsp_max;
    int time_debug_lsp_max_num;

    int time_debug_N_cnt;
    int time_debug_N_sum;
    int time_debug_N_max;
    int time_debug_N_max_num;
    
    int time_debug_N_equal_cnt;
    int time_debug_N_replace_cnt;

    int time_debug_addtent_cnt;
    int time_debug_addtent_sum;
    int time_debug_addtent_max;
    int time_debug_addtent_max_num;
#endif    
};

typedef struct tag_constraints
{
    int bandwidth;
    struct list *exclude_adjs;              /* spf算法的排除对象 */
    struct list *exclude_if_poss_adjs;      /* 尽量排除对象 */
    int hop_cnt;
    int delay;
    struct link_record *must_link;
    int root_srlg_num;
    AVLL_TREE *root_srlg_tree;

} SPF_CONSTRAINTS;

struct hop_queue
{
    struct hop_queue *next;
    struct hop_queue *prev;
    struct spf_vertex *vertex;
};

#define NBB_OFFSETOF(STRUCT, FIELD)                                           \
            (unsigned int)((unsigned char *)(&((STRUCT *)0)->FIELD) - (unsigned char *)0)

struct spftree_s * spftree_new ();

void spftree_del (struct spftree_s *spftree);

#define RET_OK 0
#define RET_ERR_NO_PATH 2

#define PASS_TYPE_ROUTER 0
#define PASS_TYPE_LINK 1

#define PASS_MAX 50                 /* 必经点+必经边最多50个. */
#define PASS_R_L_MAX (PASS_MAX * 2) /* 每个额外有个type */
#define SRLG_MAX 20
#define SRLG_LIST_STR_MAX 100
#define EDGE_FIELD_INDEX 0
#define EDGE_FIELD_FROM 1
#define EDGE_FIELD_TO 2
#define EDGE_FIELD_PEER_ID 3
#define EDGE_FIELD_COST 4
#define EDGE_FIELD_BANDWIDTH 5
#define EDGE_FIELD_DELAY 6

//#define EDGE_FIELD_NUM 6

#define SERVICE_FIELD_PATH_TYPE 0
#define SERVICE_FIELD_START 1
#define SERVICE_FIELD_END 2
#define SERVICE_FIELD_BANDWIDTH 3
#define SERVICE_FIELD_MAX_DELAY 4
#define SERVICE_FIELD_MAX_HOP 5
#define SERVICE_FIELD_MAX_MUSTPASS 6

/*****************************************************************************/
/* Flag to indicate that the resource should be excluded from the primary    */
/* route, if possible.                                                       */
/*****************************************************************************/
#define QCCS_ALWAYS_EXCLUDE 0x01

/*****************************************************************************/
/* Flag to indicate that the resource should be excluded from the route, if  */
/* possible.                                                                 */
/*****************************************************************************/
#define QCCS_EXCLUDE_IF_POSSIBLE 0x02

#define QCCS_NO_EXCLUDED_RESOURCES (char)0x00
#define QCCS_EXCLUDED_LINK (char)0x01
#define QCCS_EXCLUDED_NODE (char)0x02
#define QCCS_EXCLUDED_SRLG (char)0x04
#define QCCS_PRIMARY_LINK  (char)0x08
#define QCCS_PRIMARY_NODE  (char)0x10
#define QCCS_PRIMARY_SRLG  (char)0x20
#define QCCS_LINK_UNUSABLE (char)0x40

typedef struct node_s
{
  int id;
  int delay;
}NODE_ID;

typedef struct edge_s
{
    int edge_index;
    int from_id;
    int to_id;
    int peer_id;
    int cost;
    int bw;
    int delay;
    char srlg_list_str[SRLG_LIST_STR_MAX];
} EDGE;

typedef struct topo_s
{
    vector router_list;
    vector link_record_list;
} TOPO;

typedef struct service_demand
{
    int start_id;
    int end_id;
    int bw;
    int max_hop_num;
    int max_delay;
    vector pass_obj_array;
} SERVICE_DEMAND;

typedef struct query_s
{
    SERVICE_DEMAND primary;
    SERVICE_DEMAND backup;
} QUERY;

typedef struct pass_obj
{
  int type;
  int id;
}PASS_OBJ;

typedef struct pass_tree_node
{
    struct pass_tree_node *parent;
    struct listnode *next_pass_listnode;
    struct pass_tree_node *children;
    struct router * router;
    int calc_path;              /*true or false*/
    QCCS_TE_COST min_cost;
    int cur_path_index;
    vector cspf_path_array;
} PASS_TREENODE;

typedef struct router
{
    int id;
    int delay;
    struct list *links;         /* this router 's ADJs */
    int exclude;
    int spf_query_num;
    int cspf_query_num;
    int primary_pass_edge;                  /* 1:此节点出发有必经边 0:没有 */
    int backup_pass_edge;                   /* 1:此节点出发有必经边 0:没有 */
    struct link_record *primary_pass_link;  /* 必经边 */
    struct link_record *backup_pass_link;   /* 必经边 */
    PASS_TREENODE *pass_tnode;              /* 最近一次关联的途径树节点. not good */
    struct link_record *must_link;
}ROUTER;

typedef struct link_record
{
    int edge_index;
    ROUTER *from;
    ROUTER *to;
    int weight;
    int delay;
    unsigned int bandwidth;
    int exclude_level; //0: not exclude_level 1:exclude_level always 2:exclude_level if possible
    int query_number;
    int num_srlgs;
    int srlg_entry[SRLG_MAX];
} LINK_RECORD;

typedef struct pass_tree_s
{
    PASS_TREENODE *top;
    PASS_TREENODE *bot;
} PASS_TREE;

typedef struct query_result_s
{
    int result;//0:not begin, 1:found 2:not found
    int path_num;
    vector cspf_path_array;
}QUERY_RESULT;

typedef struct srlg_exclude
{
  /***************************************************************************/
  /* AVLL_NODE used to maintain the position within the tree.                */
  /***************************************************************************/
  AVLL_NODE avll_node;

  /***************************************************************************/
  /* The level of exclusion for this SRLG.                                   */
  /***************************************************************************/
  int exclusion_level;

  /***************************************************************************/
  /* Number of this SRLG.                                                    */
  /***************************************************************************/
  int srlg;

} QCCS_SRLG_EXCLUDE;

typedef struct query_constraints
{
    struct list *pass_router_list;  /* 必经点列表 */
    int bandwidth;
    int delay;
    int hop_cnt;
}QUERY_CONSTRAINTS;

typedef struct current_query_s
{
    int cspf_query_num;
    TOPO *topo;
    ROUTER *start;
    ROUTER *end;
    QUERY_CONSTRAINTS primary_constraints;
    QUERY_CONSTRAINTS backup_constraints;
    SPF_CONSTRAINTS constraints;
    int required_backup;
    int processing_backup;
    int hop_limit_reached;
    int delay_limit_reached;
    int minimize_hops;
    int minimize_delay;
    PASS_TREE *g_pass_tree;
    struct list *pass_tree_node_list;
    struct spftree_s *g_spftree;
    QUERY_RESULT result;
    AVLL_TREE primary_srlg_exclusion_tree;

} CURRENT_QUERY_T;


QCCS_SRLG_EXCLUDE * qccs_add_to_srlg_exclusion_tree(int srlg, int exclusion_level, AVLL_TREE *tree);

int compare_ulong(void *aa, void *bb  );

