/************************************************************************************//*!
 @file    AVLTree.cpp
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the implementation for the AVLTree object.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "AVLTree.h"
// Standard Libraries

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Default Constructor for a AVLTree.

 @param      oa
    A pointer to the object allocator to use for memory management.
 @param      ShareOA
    I don't know what this is for rn.
*//*************************************************************************************/
template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator* oa, bool ShareOA)
: BSTree<T>{oa, ShareOA}
{}

/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Inserts an element into the tree.

 @param      value
    The value of the element to insert into the tree.
*//*************************************************************************************/
template <typename T>
void AVLTree<T>::insert(const T& value)
{
    this->rootNode = insert_node(this->rootNode, value);
}
/************************************************************************************//*!
 @brief      Removes an element from the tree.

 @param      value
    The value of the element to remove from the tree.
*//*************************************************************************************/
template <typename T>
void AVLTree<T>::remove(const T& value)
{
    this->rootNode = remove_node(this->rootNode, value);
}
/************************************************************************************//*!
 @brief      Checks if balancing has been implemented.

 @returns    True if balancing is active.
*//*************************************************************************************/
template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor()
{
    return true;
}

/*-------------------------------------------------------------------------------------*/
/* Protected Function Members                                                          */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Inserts a node with a new value into the tree.

 @param      tree
    The root node of the subtree.
 @param      value
    The new value to insert.

 @returns    The root node of the subtree.
*//*************************************************************************************/
template <typename T>
typename AVLTree<T>::BinTree AVLTree<T>::insert_node(typename AVLTree<T>::BinTree& tree, const T& value)
{
    if (tree == nullptr)
        return this->make_node(value);
        
    if (value < tree->data)
    {
        ++(tree->count);
        tree->left = insert_node(tree->left, value);
    }
    else if (value > tree->data)
    {
        ++(tree->count);
        tree->right = insert_node(tree->right, value);
    }
    else
        return tree;
    
    tree->balance_factor = get_balance(tree);
    
    // Rotate Right
    if (tree->balance_factor > 1)
    {
        if (value > tree->left->data)
        {
            tree->left = rotate_left(tree->left);
        }
        return rotate_right(tree);
    }

    // Rotate Left
    if (tree->balance_factor < -1)
    {
        if (value < tree->right->data)
        {
            tree->right = rotate_right(tree->right);
        }
        return rotate_left(tree);
    }

    return tree;
}
/************************************************************************************//*!
 @brief      Removes a node with the specified from the tree.

 @param      tree
    The root node of the subtree.
 @param      value
    The value to search for to remove.

 @returns    The root node of the subtree.fr
*//*************************************************************************************/
template <typename T>
typename AVLTree<T>::BinTree AVLTree<T>::remove_node(typename AVLTree<T>::BinTree& tree, const T& value)
{
    if (tree == nullptr)
        return tree;
        
    // Smaller values traverse to the left
    if (value < tree->data)
    {
        tree->left = remove_node(tree->left, value);
        --(tree->count);
    }
    else if (value > tree->data)
    {
        tree->right = remove_node(tree->right, value);
        --(tree->count);
    }
    else 
    {
        if (tree->left == nullptr || tree->right == nullptr)
        {
            typename AVLTree<T>::BinTree temp = tree->left ? tree->left : tree->right;

            if (temp == nullptr)
            {
                temp = tree;
                tree = nullptr;
            }
            else
            {
                *tree = *temp;
            }

            this->free_node(temp);
        }
        // Both left and right child exist
        else
        {
            typename AVLTree<T>::BinTree predecessor = nullptr;
            this->find_predecessor(tree->left, predecessor);
            tree->data = predecessor->data;
            tree->left= remove_node(tree->left, predecessor->data);
            --(tree->count);
        }
    }

    if (tree == nullptr)
        return tree;
    
    tree->balance_factor = get_balance(tree);

    // Rotate Right
    if (tree->balance_factor > 1)
    {
        if (get_balance(tree->left) < 0)
        {
            tree->left = rotate_left(tree->left);
        }
        return rotate_right(tree);
    }

    // Rotate Left
    if (tree->balance_factor < -1)
    {
        if (get_balance(tree->right) > 0)
        {
            tree->right = rotate_right(tree->right);
        }

        return rotate_left(tree);
    }

    return tree;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Calculates the balance factor of the tree.

 @param      tree
    The subtree to get the balance factor of.

 @returns    The balance factor.
*//*************************************************************************************/
template <typename T>
int AVLTree<T>::get_balance(const typename BSTree<T>::BinTreeNode* tree) const
{
    if (tree == nullptr)
        return 0;

    return this->tree_height(tree->left) - this->tree_height(tree->right);
}
/************************************************************************************//*!
 @brief      Rotates a subtree on the left.

 @param      tree
    The tree to balance from.

 @returns    The balanced tree.
*//*************************************************************************************/
template <typename T>
typename AVLTree<T>::BinTree AVLTree<T>::rotate_left(typename AVLTree<T>::BinTree tree)
{
	/************
          n
         / \
        A   x
           / \
          y   B
             / \
            C   D
    ************/

    typename AVLTree<T>::BinTree x = tree->right;
    typename AVLTree<T>::BinTree y = x->left;

    x->left = tree;
    tree->right = y;

    // Fix counts
    tree->count = (tree->left ? tree->left->count : 0) + (tree->right ? tree->right->count : 0) + 1;
    x->count    = (x->left ? x->left->count : 0) + (x->right ? x->right->count : 0) + 1;

    return x;
}
/************************************************************************************//*!
 @brief      Rotates a subtree on the right.

 @param      tree
    The tree to balance from.

 @returns    The balanced tree.
*//*************************************************************************************/
template <typename T>
typename AVLTree<T>::BinTree AVLTree<T>::rotate_right(typename AVLTree<T>::BinTree tree)
{
    /************
          n
         / \
        x   A
       / \
      B   y
     / \
    C   D
    ************/

    typename AVLTree<T>::BinTree x = tree->left;
    typename AVLTree<T>::BinTree y = x->right;
    
    x->right = tree;
    tree->left = y;

    // Fix counts
    tree->count = (tree->left ? tree->left->count : 0) + (tree->right ? tree->right->count : 0) + 1;
    x->count    = (x->left ? x->left->count : 0) + (x->right ? x->right->count : 0) + 1;

    return x;
}