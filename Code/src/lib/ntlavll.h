#ifndef NTLAVLL_INCLUDED
#define NTLAVLL_INCLUDED

/**INC+***********************************************************************/
/* Header:    ntlavll.h                                                      */
/*                                                                           */
/* Purpose:   AVL Lite and AVL3 (AVL Lite Lite) tree functions               */
/*                                                                           */
/* (C) COPYRIGHT METASWITCH NETWORKS 1999 - 2012                             */
/*                                                                           */
/* $Revision:: 10002                   $$Date:: 2012-01-13 16:29:26 +0000 #$ */
/*                                                                           */
/**INC-***********************************************************************/

/*****************************************************************************/
/* AVL3 trees provide equivalent functionality to AVLL trees, with a lower   */
/* occupancy for the tree root, but require the caller to supply an          */
/* additional parameter to the macros.                                       */
/*                                                                           */
/* AVLL structures and macros are deprecated, but are retained for           */
/* back-compatibility.                                                       */
/*****************************************************************************/

/*****************************************************************************/
/* AVL3 compare function.                                                    */
/*****************************************************************************/
/*****************************************************************************/
/* compare function                                                          */
/*****************************************************************************/
typedef int (AVL_COMPARE)(void *, void *  );


typedef AVL_COMPARE AVL3_COMPARE;

/**STRUCT+********************************************************************/
/* Structure: AVL3_NODE                                                      */
/*                                                                           */
/* Description: Node in an AVL3 tree.                                        */
/*****************************************************************************/

typedef struct avl3_node
{
  struct avl3_node *parent;
  struct avl3_node *left;
  struct avl3_node *right;
  short l_height;
  short r_height;
} AVL3_NODE;

/**STRUCT-********************************************************************/

/**STRUCT+********************************************************************/
/* Structure: AVL3_TREE_INFO                                                 */
/*                                                                           */
/* Description: AVL3 tree information.                                       */
/*****************************************************************************/

typedef struct avl3_tree_info
{
  AVL3_COMPARE *compare;
  unsigned short key_offset;
  unsigned short node_offset;
} AVL3_TREE_INFO;

/**STRUCT-********************************************************************/

/**STRUCT+********************************************************************/
/* Structure: AVL3_TREE                                                      */
/*                                                                           */
/* Description: AVL3 tree root.                                              */
/*****************************************************************************/

typedef struct avl3_tree
{
  AVL3_NODE *root;
  AVL3_NODE *first;
  AVL3_NODE *last;
} AVL3_TREE;

/**STRUCT-********************************************************************/

/**STRUCT+********************************************************************/
/* Structure: AVLL_NODE                                                      */
/*                                                                           */
/* Description: Node in an AVLL tree (deprecated).                           */
/*****************************************************************************/

typedef struct avl3_node AVLL_NODE;

/**STRUCT-********************************************************************/

/**STRUCT+********************************************************************/
/* Structure: AVLL_TREE                                                      */
/*                                                                           */
/* Description: AVLL tree root (deprecated).                                 */
/*****************************************************************************/

typedef struct avll_tree
{
  AVL3_TREE tree;
  AVL3_TREE_INFO tree_info;
} AVLL_TREE;

/**STRUCT-********************************************************************/

/*****************************************************************************/
/* AVL3 functions.                                                           */
/*****************************************************************************/
 void *ntl_avl3_insert_or_find(AVL3_TREE *,
                                             AVL3_NODE *,
                                             const AVL3_TREE_INFO *
                                             );

 void ntl_avl3_delete(AVL3_TREE *, AVL3_NODE *  );

 void *ntl_avl3_find(AVL3_TREE *,
                                   void *,
                                   const AVL3_TREE_INFO *
                                   );

 void *ntl_avl3_find_or_find_next(AVL3_TREE *,
                                                void *,
                                                int,
                                                const AVL3_TREE_INFO *
                                                );

 void *ntl_avl3_first(AVL3_TREE *,
                                    const AVL3_TREE_INFO *
                                    );

 void *ntl_avl3_last(AVL3_TREE *,
                                   const AVL3_TREE_INFO *
                                   );

 void *ntl_avl3_next(AVL3_NODE *,
                                   const AVL3_TREE_INFO *
                                   );

 void *ntl_avl3_prev(AVL3_NODE *,
                                   const AVL3_TREE_INFO *
                                   );

 void ntl_avl3_verify_tree(AVL3_TREE *,
                                         const AVL3_TREE_INFO *
                                         );

/*****************************************************************************/
/* AVL3 access macros.                                                       */
/*****************************************************************************/

#define AVL3_INIT_TREE(TREE, TREE_INFO)                                       \
  (TREE).first = NULL;                                                        \
  (TREE).last = NULL;                                                         \
  (TREE).root = NULL

#define AVL3_INIT_NODE(NODE)             (NODE).parent = NULL;                \
                                         (NODE).left = NULL;                  \
                                         (NODE).right = NULL;                 \
                                         (NODE).l_height = -1;                \
                                         (NODE).r_height = -1

#define AVL3_INSERT(TREE, NODE, TREE_INFO)                                    \
    (NULL == ntl_avl3_insert_or_find(&(TREE), &(NODE), &(TREE_INFO)  ))

#define AVL3_INSERT_OR_FIND(TREE, NODE, TREE_INFO)                            \
              ntl_avl3_insert_or_find(&(TREE), &(NODE), &(TREE_INFO)  )

#define AVL3_DELETE(TREE, NODE)      ntl_avl3_delete(&(TREE), &(NODE) )

#define AVL3_FIND(TREE, KEY, TREE_INFO)                                       \
                          ntl_avl3_find(&(TREE), (KEY), &(TREE_INFO)  )

#define AVL3_NEXT(NODE, TREE_INFO)                                            \
                                 ntl_avl3_next(&(NODE), &(TREE_INFO)  )

#define AVL3_PREV(NODE, TREE_INFO)                                            \
                                 ntl_avl3_prev(&(NODE), &(TREE_INFO)  )

#define AVL3_FIRST(TREE, TREE_INFO)                                           \
                                ntl_avl3_first(&(TREE), &(TREE_INFO)  )

#define AVL3_LAST(TREE, TREE_INFO)                                            \
                                 ntl_avl3_last(&(TREE), &(TREE_INFO)  )

#define AVL3_IN_TREE(NODE) (((NODE).l_height != -1) && ((NODE).r_height != -1))

#define AVL3_FIND_NEXT(TREE, KEY, TREE_INFO)                                  \
       ntl_avl3_find_or_find_next(&(TREE), (KEY), TRUE, &(TREE_INFO)  )

#define AVL3_FIND_OR_FIND_NEXT(TREE, KEY, TREE_INFO)                          \
      ntl_avl3_find_or_find_next(&(TREE), (KEY), FALSE, &(TREE_INFO)  )

#define AVL3_MOVE_TREE(TARGET_TREE, SRC_TREE, TREE_INFO)                      \
                       (TARGET_TREE).first = (SRC_TREE).first;                \
                       (TARGET_TREE).last = (SRC_TREE).last;                  \
                       (TARGET_TREE).root = (SRC_TREE).root;                  \
                       AVL3_INIT_TREE((SRC_TREE), (TREE_INFO));

#define AVL3_VERIFY_TREE(TREE, TREE_INFO)

/*****************************************************************************/
/* AVLL access macros (deprecated).                                          */
/*****************************************************************************/
#define AVLL_INIT_TREE(TREE, COMPARE, KEY_OFF, NODE_OFF)                      \
                                  (TREE).tree_info.compare = &(COMPARE);      \
                                  (TREE).tree_info.key_offset = (KEY_OFF);    \
                                  (TREE).tree_info.node_offset = (NODE_OFF);  \
                                  AVL3_INIT_TREE((TREE).tree, (TREE).tree_info)

#define AVLL_INIT_NODE(NODE)      AVL3_INIT_NODE((NODE))

#define AVLL_INSERT(TREE, NODE)   AVL3_INSERT((TREE).tree,                    \
                                              (NODE),                         \
                                              (TREE).tree_info)

#define AVLL_INSERT_OR_FIND(TREE, NODE)                                       \
                                  AVL3_INSERT_OR_FIND((TREE).tree,            \
                                                      (NODE),                 \
                                                      (TREE).tree_info)

#define AVLL_DELETE(TREE, NODE)   AVL3_DELETE((TREE).tree, (NODE))

#define AVLL_FIND(TREE, KEY)      AVL3_FIND((TREE).tree,                      \
                                            (KEY),                            \
                                            (TREE).tree_info)

#define AVLL_NEXT(TREE, NODE)     AVL3_NEXT((NODE), (TREE).tree_info)

#define AVLL_PREV(TREE, NODE)     AVL3_PREV((NODE), (TREE).tree_info)

#define AVLL_FIRST(TREE)          AVL3_FIRST((TREE).tree, (TREE).tree_info)

#define AVLL_LAST(TREE)           AVL3_LAST((TREE).tree, (TREE).tree_info)

#define AVLL_IN_TREE(NODE)        AVL3_IN_TREE((NODE))

#define AVLL_FIND_NEXT(TREE, KEY) AVL3_FIND_NEXT((TREE).tree,                 \
                                                 (KEY),                       \
                                                 (TREE).tree_info)

#define AVLL_FIND_OR_FIND_NEXT(TREE, KEY)                                     \
                                  AVL3_FIND_OR_FIND_NEXT((TREE).tree,         \
                                                         (KEY),               \
                                                         (TREE).tree_info)

#define AVLL_VERIFY_TREE(TREE)    AVL3_VERIFY_TREE((TREE).tree,               \
                                                   (TREE).tree_info)

#endif
