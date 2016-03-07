/**MOD+***********************************************************************/
/* Module:    ntlavll.c                                                      */
/*                                                                           */
/* Purpose:   AVL Lite and AVL3 (AVL Lite Lite) tree functions               */
/*                                                                           */
/* (C) COPYRIGHT METASWITCH NETWORKS 1999 - 2012                             */
/*                                                                           */
/* $Revision:: 14307                   $$Date:: 2012-08-02 16:24:55 +0100 #$ */
/*                                                                           */
/**MOD-***********************************************************************/

#define NTL_INTERNAL
#include <base.h>
#include <ntlavll.h>

void ntl_avl3_balance_tree(AVL3_TREE *, AVL3_NODE *  _T);

void ntl_avl3_rebalance(AVL3_NODE **  _T);

void ntl_avl3_rotate_right(AVL3_NODE **  _T);

void ntl_avl3_rotate_left(AVL3_NODE **  _T);

void ntl_avl3_swap_right_most(AVL3_TREE *,
                                  AVL3_NODE *,
                                  AVL3_NODE *
                                  _T);

void ntl_avl3_swap_left_most(AVL3_TREE *,
                                 AVL3_NODE *,
                                 AVL3_NODE *
                                 _T);

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_insert_or_find                                        */
/*                                                                           */
/* Purpose:   Insert the supplied node into the specified AVL3 tree if key   */
/*            does not already exist, otherwise returning the existing node  */
/*                                                                           */
/* Returns:   void *          - Pointer to existing entry if found.      */
/*                                  NULL if no such entry (implies node      */
/*                                  successfully inserted)                   */
/*                                                                           */
/* Params:    IN/OUT tree         - a pointer to the AVL3 tree               */
/*            IN/OUT node         - a pointer to the node to insert          */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/* Operation: Scan down the tree looking for the insert point, going left if */
/*            the insert key is less than the key in the tree and right if   */
/*            it is greater.  When the insert point is found insert the new  */
/*            node and rebalance the tree if necessary.  Return the existing */
/*            entry instead, if found                                        */
/*                                                                           */
/**PROC-**********************************************************************/
void *ntl_avl3_insert_or_find(AVL3_TREE *tree,
                                  AVL3_NODE *node,
                                  const AVL3_TREE_INFO *tree_info
                                  )
{
  /***************************************************************************/
  /* insert specified node into tree                                         */
  /***************************************************************************/
  AVL3_NODE *parent_node;
  int result;
  void *existing_entry = NULL;

  NBB_TRC_ENTRY("ntl_avl3_insert_or_find");

  

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));

  node->r_height = 0;
  node->l_height = 0;

  if (tree->root == NULL)
  {
    /*************************************************************************/
    /* tree is empty, so insert at root                                      */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "tree is empty, so insert at root" ));
    tree->root = node;
    tree->first = node;
    tree->last = node;
    goto EXIT;
  }

  /***************************************************************************/
  /* scan down the tree looking for the appropriate insert point             */
  /***************************************************************************/
  NBB_TRC_FLOW((NBB_FORMAT "scan for insert point" ));
  parent_node = tree->root;
  while (parent_node != NULL)
  {
    /*************************************************************************/
    /* go left or right, depending on comparison                             */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "parent %p", parent_node));
    result = tree_info->compare((void *)((unsigned char *)node -
                                             tree_info->node_offset +
                                             tree_info->key_offset),
                                (void *)((unsigned char *)parent_node -
                                             tree_info->node_offset +
                                             tree_info->key_offset)
                                );

    if (result > 0)
    {
      /***********************************************************************/
      /* new key is greater than this node's key, so move down right subtree */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "move down right subtree" ));
      if (parent_node->right == NULL)
      {
        /*********************************************************************/
        /* right subtree is empty, so insert here                            */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "right subtree empty, insert here" ));
        
        node->parent = parent_node;
        parent_node->right = node;
        parent_node->r_height = 1;
        if (parent_node == tree->last)
        {
          /*******************************************************************/
          /* parent was the right-most node in the tree, so new node is now  */
          /* right-most                                                      */
          /*******************************************************************/
          NBB_TRC_FLOW((NBB_FORMAT "new last node" ));
          tree->last = node;
        }
        break;
      }
      else
      {
        /*********************************************************************/
        /* right subtree is not empty                                        */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "right subtree not empty" ));
        parent_node = parent_node->right;
      }
    }
    else if (result < 0)
    {
      /***********************************************************************/
      /* new key is less than this node's key, so move down left subtree     */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "move down left subtree" ));
      if (parent_node->left == NULL)
      {
        /*********************************************************************/
        /* left subtree is empty, so insert here                             */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "left subtree empty, insert here" ));
        node->parent = parent_node;
        parent_node->left = node;
        parent_node->l_height = 1;
        if (parent_node == tree->first)
        {
          /*******************************************************************/
          /* parent was the left-most node in the tree, so new node is now   */
          /* left-most                                                       */
          /*******************************************************************/
          NBB_TRC_FLOW((NBB_FORMAT "new first node" ));
          tree->first = node;
        }
        break;
      }
      else
      {
        /*********************************************************************/
        /* left subtree is not empty                                         */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "left subtree not empty" ));
        parent_node = parent_node->left;
      }
    }
    else
    {
      /***********************************************************************/
      /* found a matching key, so get out now and return entry found         */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "found matching key" ));

      existing_entry = (void *)
                            ((unsigned char *)parent_node - tree_info->node_offset);
      node->r_height = -1;
      node->l_height = -1;
      goto EXIT;
    }
  }

  /***************************************************************************/
  /* now rebalance the tree if necessary                                     */
  /***************************************************************************/
  NBB_TRC_FLOW((NBB_FORMAT "rebalance the tree" ));
  ntl_avl3_balance_tree(tree, parent_node  );

EXIT:

  NBB_TRC_EXIT();

  return(existing_entry);

} /* ntl_avl3_insert_or_find */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_delete                                                */
/*                                                                           */
/* Purpose:   Delete the specified node from the specified AVL3 tree         */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN/OUT tree         - a pointer to the AVL3 tree               */
/*            IN/OUT node         - a pointer to the node to delete          */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_delete(AVL3_TREE *tree, AVL3_NODE *node  )
{
  /***************************************************************************/
  /* delete specified node from tree                                         */
  /***************************************************************************/
  AVL3_NODE *replace_node;
  AVL3_NODE *parent_node;
  unsigned short new_height;

  NBB_TRC_ENTRY("ntl_avl3_delete");


  /***************************************************************************/
  /* Assert that the specified node is in the specified tree.  This is done  */
  /* by following the parent pointers from the specified node to the root    */
  /* node then asserting that this is the root node for the specified tree.  */
  /* Assert failure indicates one of the following:                          */
  /* -  The caller is attempting to delete a node from a tree rooted in a    */
  /*    control block that has been freed.                                   */
  /*    Without the assert this risks memory corruption.                     */
  /* -  The caller is attempting to delete a node from the wrong tree.       */
  /*    Without the assert this would silently corrupt one or both trees.    */
  /* -  The tree is already corrupt.                                         */
  /***************************************************************************/

  if ((node->left == NULL) &&
      (node->right == NULL))
  {
    /*************************************************************************/
    /* barren node (no children), so just delete it                          */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "delete barren node" ));
    replace_node = NULL;

    if (tree->first == node)
    {
      /***********************************************************************/
      /* node was first in tree, so replace it                               */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "replace first node in tree" ));
      tree->first = node->parent;
    }

    if (tree->last == node)
    {
      /***********************************************************************/
      /* node was last in tree, so replace it                                */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "replace last node in tree" ));
      tree->last = node->parent;
    }
  }
  else if (node->left == NULL)
  {
    /*************************************************************************/
    /* node has no left son, so replace with right son                       */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "node has no left son, replace with right son" ));
    replace_node = node->right;

    if (tree->first == node)
    {
      /***********************************************************************/
      /* node was first in tree, so replace it                               */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "replace first node in tree" ));
      tree->first = replace_node;
    }
  }
  else if (node->right == NULL)
  {
    /*************************************************************************/
    /* node has no right son, so replace with left son                       */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "node has no right son, replace with left son" ));
    replace_node = node->left;

    if (tree->last == node)
    {
      /***********************************************************************/
      /* node was last in tree, so replace it                                */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "replace last node in tree" ));
      tree->last = replace_node;
    }
  }
  else
  {
    /*************************************************************************/
    /* node has both left and right-sons                                     */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "node has two sons" ));
    if (node->r_height > node->l_height)
    {
      /***********************************************************************/
      /* right subtree is higher than left subtree                           */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "right subtree is higher" ));
      if (node->right->left == NULL)
      {
        /*********************************************************************/
        /* can replace node with right-son (since it has no left-son)        */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "replace node with right son" ));
        replace_node = node->right;
        replace_node->left = node->left;
        replace_node->left->parent = replace_node;
        replace_node->l_height = node->l_height;
      }
      else
      {
        /*********************************************************************/
        /* swap with left-most descendent of right subtree                   */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "swap with left-most right descendent" ));
        ntl_avl3_swap_left_most(tree, node->right, node  );
        replace_node = node->right;
      }
    }
    else
    {
      /***********************************************************************/
      /* left subtree is higher (or subtrees are of same height)             */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "left subtree is higher" ));
      NBB_TRC_FLOW((NBB_FORMAT "(or both subtrees are of equal height)" ));
      if (node->left->right == NULL)
      {
        /*********************************************************************/
        /* can replace node with left-son (since it has no right-son)        */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "replace node with left son" ));
        replace_node = node->left;
        replace_node->right = node->right;
        replace_node->right->parent = replace_node;
        replace_node->r_height = node->r_height;
      }
      else
      {
        /*********************************************************************/
        /* swap with right-most descendent of left subtree                   */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "swap with right-most left descendent" ));
        ntl_avl3_swap_right_most(tree, node->left, node  );
        replace_node = node->left;
      }
    }
  }

  /***************************************************************************/
  /* save parent node of deleted node                                        */
  /***************************************************************************/
  parent_node = node->parent;

  /***************************************************************************/
  /* reset deleted node                                                      */
  /***************************************************************************/
  NBB_TRC_FLOW((NBB_FORMAT "reset deleted node" ));
  node->parent = NULL;
  node->right = NULL;
  node->left = NULL;
  node->r_height = -1;
  node->l_height = -1;

  if (replace_node != NULL)
  {
    /*************************************************************************/
    /* fix-up parent pointer of replacement node, and calculate new height   */
    /* of subtree                                                            */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "fixup parent pointer of replacement node" ));
    replace_node->parent = parent_node;
    new_height = (unsigned short)
                 (1 + NBB_MAX(replace_node->l_height, replace_node->r_height));
  }
  else
  {
    /*************************************************************************/
    /* no replacement, so new height of subtree is zero                      */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "new height zero"));
    new_height = 0;
  }
  NBB_TRC_FLOW((NBB_FORMAT "new height of parent is %d", new_height ));

  if (parent_node != NULL)
  {
    /*************************************************************************/
    /* fixup parent node                                                     */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "fix-up parent node" ));
    if (parent_node->right == node)
    {
      /***********************************************************************/
      /* node is right son of parent                                         */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "replacement node is right son" ));
      parent_node->right = replace_node;
      parent_node->r_height = new_height;
    }
    else
    {
      /***********************************************************************/
      /* node is left son of parent                                          */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "replacement node is left son" ));
      parent_node->left = replace_node;
      parent_node->l_height = new_height;
    }

    /*************************************************************************/
    /* now rebalance the tree (if necessary)                                 */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "rebalance the tree" ));
    ntl_avl3_balance_tree(tree, parent_node  );
  }
  else
  {
    /*************************************************************************/
    /* replacement node is now root of tree                                  */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "replacement node is now root of tree" ));
    tree->root = replace_node;
  }

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_delete */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_find                                                  */
/*                                                                           */
/* Purpose:   Find the node in the AVL3 tree with the supplied key           */
/*                                                                           */
/* Returns:   A pointer to the node                                          */
/*            NULL if no node is found with the specified key                */
/*                                                                           */
/* Params:    IN     tree         - a pointer to the AVL3 tree               */
/*            IN     key          - a pointer to the key                     */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/* Operation: Search down the tree going left if the search key is less than */
/*            the node in the tree and right if the search key is greater.   */
/*            When we run out of tree to search through either we've found   */
/*            it or the node is not in the tree.                             */
/*                                                                           */
/**PROC-**********************************************************************/
void *ntl_avl3_find(AVL3_TREE *tree,
                        void *key,
                        const AVL3_TREE_INFO *tree_info
                        )
{
  /***************************************************************************/
  /* find node with specified key                                            */
  /***************************************************************************/
  AVL3_NODE *node;
  int result;

  NBB_TRC_ENTRY("ntl_avl3_find");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));

  node = tree->root;

  while (node != NULL)
  {
    /*************************************************************************/
    /* compare key of current node with supplied key                         */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "node %p", node));
    result = tree_info->compare(key,
                                (void *)((unsigned char *)node -
                                             tree_info->node_offset +
                                             tree_info->key_offset)
                                );

    if (result > 0)
    {
      /***********************************************************************/
      /* specified key is greater than key of this node, so look in right    */
      /* subtree                                                             */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "move down right subtree" ));
      node = node->right;
    }
    else if (result < 0)
    {
      /***********************************************************************/
      /* specified key is less than key of this node, so look in left        */
      /* subtree                                                             */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "move down left subtree" ));
      node = node->left;
    }
    else
    {
      /***********************************************************************/
      /* found the requested node                                            */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "found requested node" ));
      break;
    }
  }

  NBB_TRC_EXIT();

  return((node != NULL) ?
               (void *)((unsigned char *)node - tree_info->node_offset) : NULL);

} /* ntl_avl3_find */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_first                                                 */
/*                                                                           */
/* Purpose:   Find the first entry in the AVL3 tree.                         */
/*                                                                           */
/* Returns:   A pointer to the first entry.  NULL if the tree is empty.      */
/*                                                                           */
/* Params:    IN     tree         - a pointer to the AVL3 tree               */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/**PROC-**********************************************************************/
void *ntl_avl3_first(AVL3_TREE *tree,
                         const AVL3_TREE_INFO *tree_info
                         )
{
  /***************************************************************************/
  /* find first node in tree                                                 */
  /***************************************************************************/
  AVL3_NODE *node;

  NBB_TRC_ENTRY("ntl_avl3_first");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));

  if (tree->first != NULL)
  {
    NBB_TRC_FLOW((NBB_FORMAT "Tree not empty"));
    node = tree->first;
  }
  else
  {
    NBB_TRC_FLOW((NBB_FORMAT "Tree empty"));
    node = NULL;
  }

  NBB_TRC_EXIT();

  return((node != NULL) ?
               (void *)((unsigned char *)node - tree_info->node_offset) : NULL);

} /* ntl_avl3_first */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_last                                                  */
/*                                                                           */
/* Purpose:   Find the last entry in the AVL3 tree.                          */
/*                                                                           */
/* Returns:   A pointer to the last entry.  NULL if the tree is empty.       */
/*                                                                           */
/* Params:    IN     tree         - a pointer to the AVL3 tree               */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/**PROC-**********************************************************************/
void *ntl_avl3_last(AVL3_TREE *tree,
                        const AVL3_TREE_INFO *tree_info
                        )
{
  /***************************************************************************/
  /* find last node in tree                                                  */
  /***************************************************************************/
  AVL3_NODE *node;

  NBB_TRC_ENTRY("ntl_avl3_last");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));

  if (tree->last != NULL)
  {
    NBB_TRC_FLOW((NBB_FORMAT "Tree not empty"));
    node = tree->last;
  }
  else
  {
    NBB_TRC_FLOW((NBB_FORMAT "Tree empty"));
    node = NULL;
  }

  NBB_TRC_EXIT();

  return((node != NULL) ?
               (void *)((unsigned char *)node - tree_info->node_offset) : NULL);

} /* ntl_avl3_last */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_next                                                  */
/*                                                                           */
/* Purpose:   Find next node in the AVL3 tree                                */
/*                                                                           */
/* Returns:   A pointer to the next node in the tree                         */
/*                                                                           */
/* Params:    IN     node         - a pointer to the current node in the     */
/*                                  tree                                     */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/* Operation: If the specified node has a right-son then return the left-    */
/*            most son of this.  Otherwise search back up until we find a    */
/*            node of which we are in the left sub-tree and return that.     */
/*                                                                           */
/**PROC-**********************************************************************/
void *ntl_avl3_next(AVL3_NODE *node,
                        const AVL3_TREE_INFO *tree_info
                        )
{
  /***************************************************************************/
  /* find next node in tree                                                  */
  /***************************************************************************/
  NBB_TRC_ENTRY("ntl_avl3_next");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));



  if (node->right != NULL)
  {
    /*************************************************************************/
    /* next node is left-most node in right subtree                          */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "next node is left-most right descendent" ));
    node = node->right;
    while (node->left != NULL)
    {
      /***********************************************************************/
      /* FLOW TRACING NOT REQUIRED    Reason: Tight loop.                    */
      /***********************************************************************/
      node = node->left;
    }
  }
  else
  {
    /*************************************************************************/
    /* no right-son, so find a node of which we are in the left subtree      */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "find node which this is in left subtree of" ));

    while (node != NULL)
    {
      /***********************************************************************/
      /* FLOW TRACING NOT REQUIRED    Reason: Tight loop.                    */
      /***********************************************************************/
      if ((node->parent == NULL) ||
          (node->parent->left == node))
      {
        NBB_TRC_FLOW((NBB_FORMAT "found node %p", node));
        node = node->parent;
        break;
      }
      node = node->parent;
    }
  }

  NBB_TRC_EXIT();

  return((node != NULL) ?
               (void *)((unsigned char *)node - tree_info->node_offset) : NULL);

} /* ntl_avl3_next */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_prev                                                  */
/*                                                                           */
/* Purpose:   Find previous node in the AVL3 tree                            */
/*                                                                           */
/* Returns:   A pointer to the previous node in the tree                     */
/*                                                                           */
/* Params:    IN     node         - a pointer to the current node in the     */
/*                                  tree                                     */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/* Operation: If we have a left-son then the previous node is the right-most */
/*            son of this.  Otherwise, look for a node of whom we are in the */
/*            left subtree and return that.                                  */
/*                                                                           */
/**PROC-**********************************************************************/
void *ntl_avl3_prev(AVL3_NODE *node,
                        const AVL3_TREE_INFO *tree_info
                        )
{
  /***************************************************************************/
  /* find previous node in tree                                              */
  /***************************************************************************/
  NBB_TRC_ENTRY("ntl_avl3_prev");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));



  if (node->left != NULL)
  {
    /*************************************************************************/
    /* previous node is right-most node in left subtree                      */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "previous node is right-most left descendent" ));
    node = node->left;
    while (node->right != NULL)
    {
      /***********************************************************************/
      /* FLOW TRACING NOT REQUIRED    Reason: Tight loop.                    */
      /***********************************************************************/
      node = node->right;
    }
  }
  else
  {
    /*************************************************************************/
    /* no left-son, so find a node of which we are in the right subtree      */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "find node which this is in right subtree of"));
    while (node != NULL)
    {
      /***********************************************************************/
      /* FLOW TRACING NOT REQUIRED    Reason: Tight loop.                    */
      /***********************************************************************/
      if ((node->parent == NULL) ||
          (node->parent->right == node))
      {
        NBB_TRC_FLOW((NBB_FORMAT "found node %p", node));
        node = node->parent;
        break;
      }
      node = node->parent;
    }
  }

  NBB_TRC_EXIT();

  return((node != NULL) ?
               (void *)((unsigned char *)node - tree_info->node_offset) : NULL);

} /* ntl_avl3_prev */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_balance_tree                                          */
/*                                                                           */
/* Purpose:   Rebalance the tree starting at the supplied node and ending at */
/*            the root of the tree                                           */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN/OUT tree              - a pointer to the AVL3 tree          */
/*            IN/OUT node              - a pointer to the node to start      */
/*                                       balancing from                      */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_balance_tree(AVL3_TREE *tree,
                               AVL3_NODE *node
                               )
{
  /***************************************************************************/
  /* balance the tree starting at the supplied node, and ending at the root  */
  /* of the tree                                                             */
  /***************************************************************************/
  NBB_TRC_ENTRY("ntl_avl3_balance_tree");

  while (node->parent != NULL)
  {
    /*************************************************************************/
    /* node has uneven balance, so may need to rebalance it                  */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "check node balance" ));
    NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d", node->r_height ));
    NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d", node->l_height ));

    if (node->parent->right == node)
    {
      /***********************************************************************/
      /* node is right-son of its parent                                     */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "node is right-son" ));
      node = node->parent;
      ntl_avl3_rebalance(&node->right  );

      /***********************************************************************/
      /* now update the right height of the parent                           */
      /***********************************************************************/
      node->r_height = (unsigned short)
                   (1 + NBB_MAX(node->right->r_height, node->right->l_height));
      NBB_TRC_DETAIL((NBB_FORMAT "new r_height = %d", node->r_height ));
    }
    else
    {
      /***********************************************************************/
      /* node is left-son of its parent                                      */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "node is left-son" ));
      node = node->parent;
      ntl_avl3_rebalance(&node->left  );

      /***********************************************************************/
      /* now update the left height of the parent                            */
      /***********************************************************************/
      node->l_height = (unsigned short)
                     (1 + NBB_MAX(node->left->r_height, node->left->l_height));
      NBB_TRC_DETAIL((NBB_FORMAT "new l_height = %d", node->l_height ));
    }
  }

  if (node->l_height != node->r_height)
  {
    /*************************************************************************/
    /* rebalance root node                                                   */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "rebalance root node" ));
    ntl_avl3_rebalance(&tree->root  );
  }

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_balance_tree */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_rebalance                                             */
/*                                                                           */
/* Purpose:   Rebalance a subtree of the AVL3 tree (if necessary)            */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN/OUT subtree           - a pointer to the subtree to         */
/*                                       rebalance                           */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_rebalance(AVL3_NODE **subtree  )
{
  /***************************************************************************/
  /* Local data                                                              */
  /***************************************************************************/
  int moment;

  /***************************************************************************/
  /* rebalance a subtree of the AVL3 tree                                    */
  /***************************************************************************/
  NBB_TRC_ENTRY("ntl_avl3_rebalance");

  NBB_TRC_FLOW((NBB_FORMAT "rebalance subtree" ));
  NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d", (*subtree)->r_height ));
  NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d", (*subtree)->l_height ));

  /***************************************************************************/
  /* How unbalanced - don't want to recalculate                              */
  /***************************************************************************/
  moment = (*subtree)->r_height - (*subtree)->l_height;

  if (moment > 1)
  {
    /*************************************************************************/
    /* subtree is heavy on the right side                                    */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "subtree is heavy on right side" ));
    NBB_TRC_DETAIL((NBB_FORMAT "right subtree" ));
    NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d",
                                                 (*subtree)->right->r_height));
    NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d",
                                                 (*subtree)->right->l_height));
    if ((*subtree)->right->l_height > (*subtree)->right->r_height)
    {
      /***********************************************************************/
      /* right subtree is heavier on left side, so must perform right        */
      /* rotation on this subtree to make it heavier on the right side       */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "right subtree is heavier on left side ..." ));
      NBB_TRC_FLOW((NBB_FORMAT "... so rotate it right" ));
      ntl_avl3_rotate_right(&(*subtree)->right  );
      NBB_TRC_DETAIL((NBB_FORMAT "right subtree" ));
      NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d",
                                                 (*subtree)->right->r_height));
      NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d",
                                                 (*subtree)->right->l_height));
    }

    /*************************************************************************/
    /* now rotate the subtree left                                           */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "rotate subtree left" ));
    ntl_avl3_rotate_left(subtree  );
  }
  else if (moment < -1)
  {
    /*************************************************************************/
    /* subtree is heavy on the left side                                     */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "subtree is heavy on left side" ));
    NBB_TRC_DETAIL((NBB_FORMAT "left subtree" ));
    NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d", (*subtree)->left->r_height));
    NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d", (*subtree)->left->l_height));
    if ((*subtree)->left->r_height > (*subtree)->left->l_height)
    {
      /***********************************************************************/
      /* left subtree is heavier on right side, so must perform left         */
      /* rotation on this subtree to make it heavier on the left side        */
      /***********************************************************************/
      NBB_TRC_FLOW((NBB_FORMAT "left subtree is heavier on right side ..." ));
      NBB_TRC_FLOW((NBB_FORMAT "... so rotate it left" ));
      ntl_avl3_rotate_left(&(*subtree)->left  );
      NBB_TRC_DETAIL((NBB_FORMAT "left subtree" ));
      NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d",
                                                  (*subtree)->left->r_height));
      NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d",
                                                  (*subtree)->left->l_height));
    }

    /*************************************************************************/
    /* now rotate the subtree right                                          */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "rotate subtree right" ));
    ntl_avl3_rotate_right(subtree  );
  }

  NBB_TRC_DETAIL((NBB_FORMAT "balanced subtree" ));
  NBB_TRC_DETAIL((NBB_FORMAT "  r_height = %d", (*subtree)->r_height ));
  NBB_TRC_DETAIL((NBB_FORMAT "  l_height = %d", (*subtree)->l_height ));

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_rebalance */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_rotate_right                                          */
/*                                                                           */
/* Purpose:   Rotate a subtree of the AVL3 tree right                        */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN/OUT subtree           - a pointer to the subtree to rotate  */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_rotate_right(AVL3_NODE **subtree  )
{
  /***************************************************************************/
  /* rotate subtree of AVL3 tree right                                       */
  /***************************************************************************/
  AVL3_NODE *left_son;

  NBB_TRC_ENTRY("ntl_avl3_rotate_right");

  left_son = (*subtree)->left;

  (*subtree)->left = left_son->right;
  if ((*subtree)->left != NULL)
  {
    NBB_TRC_FLOW((NBB_FORMAT "left son exists"));
    (*subtree)->left->parent = (*subtree);
  }
  (*subtree)->l_height = left_son->r_height;

  left_son->parent = (*subtree)->parent;

  left_son->right = *subtree;
  left_son->right->parent = left_son;
  left_son->r_height = (unsigned short)
                     (1 + NBB_MAX((*subtree)->r_height, (*subtree)->l_height));

  *subtree = left_son;

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_rotate_right */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_rotate_left                                           */
/*                                                                           */
/* Purpose:   Rotate a subtree of the AVL3 tree left                         */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN/OUT subtree           - a pointer to the subtree to rotate  */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_rotate_left(AVL3_NODE **subtree  )
{
  /***************************************************************************/
  /* rotate a subtree of the AVL tree left                                   */
  /***************************************************************************/
  AVL3_NODE *right_son;

  NBB_TRC_ENTRY("ntl_avl3_rotate_left");

  right_son = (*subtree)->right;

  (*subtree)->right = right_son->left;
  if ((*subtree)->right != NULL)
  {
    NBB_TRC_FLOW((NBB_FORMAT "right son exists"));
    (*subtree)->right->parent = (*subtree);
  }
  (*subtree)->r_height = right_son->l_height;

  right_son->parent = (*subtree)->parent;

  right_son->left = *subtree;
  right_son->left->parent = right_son;
  right_son->l_height = (unsigned short)
                     (1 + NBB_MAX((*subtree)->r_height, (*subtree)->l_height));

  *subtree = right_son;

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_rotate_left */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_swap_right_most                                       */
/*                                                                           */
/* Purpose:   Swap node with right-most descendent of subtree                */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN     tree              - a pointer to the tree               */
/*            IN     subtree           - a pointer to the subtree            */
/*            IN     node              - a pointer to the node to swap       */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_swap_right_most(AVL3_TREE *tree,
                                  AVL3_NODE *subtree,
                                  AVL3_NODE *node
                                  )
{
  /***************************************************************************/
  /* swap node with right-most descendent of specified subtree               */
  /***************************************************************************/
  AVL3_NODE *swap_node;
  AVL3_NODE *swap_parent;
  AVL3_NODE *swap_left;

  NBB_TRC_ENTRY("ntl_avl3_swap_right_most");


  /***************************************************************************/
  /* find right-most descendent of subtree                                   */
  /***************************************************************************/
  swap_node = subtree;
  while (swap_node->right != NULL)
  {
    /*************************************************************************/
    /* FLOW TRACING NOT REQUIRED    Reason: Tight loop.                      */
    /*************************************************************************/
    swap_node = swap_node->right;
  }


  /***************************************************************************/
  /* save parent and left-son of right-most descendent                       */
  /***************************************************************************/
  swap_parent = swap_node->parent;
  swap_left = swap_node->left;

  /***************************************************************************/
  /* move swap node to its new position                                      */
  /***************************************************************************/
  swap_node->parent = node->parent;
  swap_node->right = node->right;
  swap_node->left = node->left;
  swap_node->r_height = node->r_height;
  swap_node->l_height = node->l_height;
  swap_node->right->parent = swap_node;
  swap_node->left->parent = swap_node;
  if (node->parent == NULL)
  {
    /*************************************************************************/
    /* node is at root of tree                                               */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Node is root"));
    tree->root = swap_node;
  }
  else if (node->parent->right == node)
  {
    /*************************************************************************/
    /* node is right-son of parent                                           */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Node is right-son"));
    swap_node->parent->right = swap_node;
  }
  else
  {
    /*************************************************************************/
    /* node is left-son of parent                                            */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Node is left-son"));
    swap_node->parent->left = swap_node;
  }

  /***************************************************************************/
  /* move node to its new position                                           */
  /***************************************************************************/
  node->parent = swap_parent;
  node->right = NULL;
  node->left = swap_left;
  if (node->left != NULL)
  {
    NBB_TRC_FLOW((NBB_FORMAT "node has left-son"));
    node->left->parent = node;
    node->l_height = 1;
  }
  else
  {
    NBB_TRC_FLOW((NBB_FORMAT "node has no left-son"));
    node->l_height = 0;
  }
  node->r_height = 0;
  node->parent->right = node;

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_swap_right_most */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_swap_left_most                                        */
/*                                                                           */
/* Purpose:   Swap node with left-most descendent of subtree                 */
/*                                                                           */
/* Returns:   Nothing                                                        */
/*                                                                           */
/* Params:    IN     tree              - a pointer to the tree               */
/*            IN     subtree           - a pointer to the subtree            */
/*            IN     node              - a pointer to the node to swap       */
/*                                                                           */
/**PROC-**********************************************************************/
void ntl_avl3_swap_left_most(AVL3_TREE *tree,
                                 AVL3_NODE *subtree,
                                 AVL3_NODE *node
                                 )
{
  /***************************************************************************/
  /* swap node with left-most descendent of specified subtree                */
  /***************************************************************************/
  AVL3_NODE *swap_node;
  AVL3_NODE *swap_parent;
  AVL3_NODE *swap_right;

  NBB_TRC_ENTRY("ntl_avl3_swap_left_most");


  /***************************************************************************/
  /* find left-most descendent of subtree                                    */
  /***************************************************************************/
  swap_node = subtree;
  while (swap_node->left != NULL)
  {
    /*************************************************************************/
    /* FLOW TRACING NOT REQUIRED    Reason: Tight loop.                      */
    /*************************************************************************/
    swap_node = swap_node->left;
  }


  /***************************************************************************/
  /* save parent and right-son of left-most descendent                       */
  /***************************************************************************/
  swap_parent = swap_node->parent;
  swap_right = swap_node->right;

  /***************************************************************************/
  /* move swap node to its new position                                      */
  /***************************************************************************/
  swap_node->parent = node->parent;
  swap_node->right = node->right;
  swap_node->left = node->left;
  swap_node->r_height = node->r_height;
  swap_node->l_height = node->l_height;
  swap_node->right->parent = swap_node;
  swap_node->left->parent = swap_node;
  if (node->parent == NULL)
  {
    /*************************************************************************/
    /* node is at root of tree                                               */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Node is root"));
    tree->root = swap_node;
  }
  else if (node->parent->right == node)
  {
    /*************************************************************************/
    /* node is right-son of parent                                           */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Node is right-son"));
    swap_node->parent->right = swap_node;
  }
  else
  {
    /*************************************************************************/
    /* node is left-son of parent                                            */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Node is left-son"));
    swap_node->parent->left = swap_node;
  }

  /***************************************************************************/
  /* move node to its new position                                           */
  /***************************************************************************/
  node->parent = swap_parent;
  node->right = swap_right;
  node->left = NULL;
  if (node->right != NULL)
  {
    NBB_TRC_FLOW((NBB_FORMAT "node has right-son"));
    node->right->parent = node;
    node->r_height = 1;
  }
  else
  {
    NBB_TRC_FLOW((NBB_FORMAT "node has no right-son"));
    node->r_height = 0;
  }
  node->l_height = 0;
  node->parent->left = node;

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_swap_left_most */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_find_or_find_next                                     */
/*                                                                           */
/* Purpose:   Find the successor node to the supplied key in the AVL3 tree   */
/*                                                                           */
/* Returns:   A pointer to the node                                          */
/*            NULL if no successor node to the supplied key is found         */
/*                                                                           */
/* Params:    IN     tree         - a pointer to the AVL3 tree               */
/*            IN     key          - a pointer to the key                     */
/*            IN     not_equal    - TRUE return a node strictly > key FALSE  */
/*                                  return a node >= key                     */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/**PROC-**********************************************************************/

void *ntl_avl3_find_or_find_next(AVL3_TREE *tree,
                                     void *key,
                                     int not_equal,
                                     const AVL3_TREE_INFO *tree_info
                                     )
{
  /***************************************************************************/
  /* Local Variables                                                         */
  /***************************************************************************/
  AVL3_NODE *node;
  void *found_node = NULL;
  int result;

  NBB_TRC_ENTRY("ntl_avl3_find_or_find_next");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));

  node = tree->root;

  if (node != NULL)
  {
    /*************************************************************************/
    /* There is something in the tree                                        */
    /*************************************************************************/
    NBB_TRC_FLOW((NBB_FORMAT "Tree not empty"));
    for (;;)
    {
      /***********************************************************************/
      /* FLOW TRACING NOT REQUIRED           Reason: Performance             */
      /*                                                                     */
      /* compare key of current node with supplied key                       */
      /***********************************************************************/
      result = tree_info->compare(key,
                                  (void *)((unsigned char *)node -
                                               tree_info->node_offset +
                                               tree_info->key_offset)
                                  );

      if (result > 0)
      {
        /*********************************************************************/
        /* specified key is greater than key of this node, so look in right  */
        /* subtree                                                           */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "move down right subtree" ));
        if (node->right == NULL)
        {
          /*******************************************************************/
          /* We've found the previous node - so we now need to find the      */
          /* successor to this one.                                          */
          /*******************************************************************/
          NBB_TRC_FLOW((NBB_FORMAT "Found previous node"));
          found_node = ntl_avl3_next(node, tree_info  );
          break;
        }
        node = node->right;
      }
      else if (result < 0)
      {
        /*********************************************************************/
        /* specified key is less than key of this node, so look in left      */
        /* subtree                                                           */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "move down left subtree" ));

        if (node->left == NULL)
        {
           /******************************************************************/
           /* We've found the next node so store and drop out                */
           /******************************************************************/
           NBB_TRC_FLOW((NBB_FORMAT "Found next node"));
           found_node = (void *)
                                   ((unsigned char *)node - tree_info->node_offset);
           break;
        }
        node = node->left;
      }
      else
      {
        /*********************************************************************/
        /* found the requested node                                          */
        /*********************************************************************/
        NBB_TRC_FLOW((NBB_FORMAT "found node for suplied key" ));

        if (not_equal)
        {
          /*******************************************************************/
          /* need to find the successor node to this node                    */
          /*******************************************************************/
          NBB_TRC_FLOW((NBB_FORMAT "next node required"));
          found_node = ntl_avl3_next(node, tree_info  );
        }
        else
        {
          NBB_TRC_FLOW((NBB_FORMAT "return the exact match"));
          found_node = (void *)((unsigned char *)node - tree_info->node_offset);
        }
        break;
      }
    }
  }

  NBB_TRC_EXIT();

  return(found_node);

} /* ntl_avl3_find_or_find_next */

/**PROC+**********************************************************************/
/* Name:      ntl_avl3_verify_tree                                           */
/*                                                                           */
/* Purpose:   To verify that an AVL3 tree is correctly sorted.               */
/*                                                                           */
/* Returns:   Nothing.                                                       */
/*                                                                           */
/* Params:    IN     tree         - a pointer to the AVL3 tree               */
/*            IN     tree_info    - a pointer to the AVL3 tree info          */
/*                                                                           */
/**PROC-**********************************************************************/

void ntl_avl3_verify_tree(AVL3_TREE *tree,
                              const AVL3_TREE_INFO *tree_info
                              )
{
  /***************************************************************************/
  /* Local Variables                                                         */
  /***************************************************************************/
  AVL3_NODE *node = NULL;
  AVL3_NODE *tmpnode = NULL;
  int result;

  NBB_TRC_ENTRY("ntl_avl3_verify_tree");

  NBB_TRC_DETAIL((NBB_FORMAT "Compare fn %p; key offset %hu; node offset %hu",
                  tree_info->compare,
                  tree_info->key_offset,
                  tree_info->node_offset));

  /***************************************************************************/
  /* Assert that the root node has no parent.                                */
  /***************************************************************************/

  /***************************************************************************/
  /* The first node is the leftmost node in the tree - following the left    */
  /* pointers from the root node ends at the first node, and following the   */
  /* parent pointers from the first node ends at the root node.              */
  /*                                                                         */
  /* Check that this chain from the first node to the root node is correct   */
  /* by following the parent pointers from the first node to the root node   */
  /* and asserting the left pointers at each step.                           */
  /*                                                                         */
  /* Within the loop node is the parent of tmpnode and tmpnode should be the */
  /* left child of node.  At the end of the loop node is NULL and tmpnode    */
  /* should be the root node.                                                */
  /***************************************************************************/
  tmpnode = NULL;
  for (node = tree->first; node != NULL; node = node->parent)
  {
    /*************************************************************************/
    /* FLOW TRACING NOT REQUIRED Reason: Performance                         */
    /*************************************************************************/
    tmpnode = node;
  }

  /***************************************************************************/
  /* Now we've ascertained that the first and root are correct, check the    */
  /* relative ordering of everything else in the tree.                       */
  /***************************************************************************/
  node = tree->first;

  while (node != NULL)
  {
    /*************************************************************************/
    /* FLOW TRACING NOT REQUIRED Reason: Performance                         */
    /*************************************************************************/
    tmpnode = (AVL3_NODE *)AVL3_NEXT(*node, *tree_info);

    /*************************************************************************/
    /* Get out if we have reach the end of the line.                         */
    /*************************************************************************/
    if (tmpnode == NULL)
    {
      /***********************************************************************/
      /* FLOW TRACING NOT REQUIRED Reason: Performance                       */
      /***********************************************************************/
      break;
    }

    /*************************************************************************/
    /* Check that node and its next node are ordered correctly.  Check the   */
    /* ordering in both directions to ensure there are no infinite loops.    */
    /*************************************************************************/
    tmpnode = (AVL3_NODE *)(((unsigned char *)tmpnode + tree_info->node_offset));
    result = tree_info->compare((void *)((unsigned char *)node -
                                             tree_info->node_offset +
                                             tree_info->key_offset),
                                (void *)((unsigned char *)tmpnode -
                                             tree_info->node_offset +
                                             tree_info->key_offset)
                                );
    result = tree_info->compare((void *)((unsigned char *)tmpnode -
                                             tree_info->node_offset +
                                             tree_info->key_offset),
                                (void *)((unsigned char *)node -
                                             tree_info->node_offset +
                                             tree_info->key_offset)
                                );

    /*************************************************************************/
    /* Also check the parentage.                                             */
    /*************************************************************************/
    node = tmpnode;
  }

  /***************************************************************************/
  /* Finally, now we have completed the walk of the (potentially empty)      */
  /* tree, check that the last pointer is set correctly.                     */
  /***************************************************************************/

  /***************************************************************************/
  /* There is no need to walk through last's parentage to check them since   */
  /* we have already checked the correct ordering of all members of the tree */
  /* while we were walking.  This step is necessary for the first since it   */
  /* it the foundation upon which we base our tree walk to check all the     */
  /* relative orderings.                                                     */
  /***************************************************************************/

  NBB_TRC_EXIT();

  return;

} /* ntl_avl3_verify_tree */
