/************************************************************************************//*!
 @file    AVLTree.h
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the interface for the AVLTree object.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef AVLTREE_H
#define AVLTREE_H
#include <stack>
#include "BSTree.h"

/************************************************************************************//*!
 @brief     Encapsulates an AVLTree. Inherits from BSTree.

 @tparam    T
    The type of an element in the AVLTree.
*//*************************************************************************************/
template <typename T>
class AVLTree : public BSTree<T>
{
public:
    using BinTree = typename BSTree<T>::BinTree;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Default Constructor for a AVLTree.

    @param      oa
        A pointer to the object allocator to use for memory management.
    @param      ShareOA
        I don't know what this is for rn.
    *//*********************************************************************************/
    AVLTree(ObjectAllocator* oa = nullptr, bool ShareOA = false);
    /********************************************************************************//*!
    @brief      Destructor for a BSTree.
    *//*********************************************************************************/
    virtual ~AVLTree() = default;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Inserts an element into the tree.

    @param      value
        The value of the element to insert into the tree.
    *//*********************************************************************************/
    virtual void insert(const T& value) override;
    /********************************************************************************//*!
    @brief      Removes an element from the tree.

    @param      value
        The value of the element to remove from the tree.
    *//*********************************************************************************/
    virtual void remove(const T& value) override;
    /********************************************************************************//*!
    @brief      Checks if balancing has been implemented.

    @returns    True if balancing is active.
    *//*********************************************************************************/
    static bool ImplementedBalanceFactor(void);

protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Inserts a node with a new value into the tree.

    @param      tree
        The root node of the subtree.
    @param      value
        The new value to insert.

    @returns    The root node of the subtree.
    *//*********************************************************************************/
    virtual BinTree insert_node (BinTree& tree, const T& value) override;
    /********************************************************************************//*!
    @brief      Removes a node with the specified from the tree.

    @param      tree
        The root node of the subtree.
    @param      value
        The value to search for to remove.

    @returns    The root node of the subtree.fr
    *//*********************************************************************************/
    virtual BinTree remove_node (BinTree& tree, const T& value) override;

private:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Calculates the balance factor of the tree.

    @param      tree
        The subtree to get the balance factor of.

    @returns    The balance factor.
    *//*********************************************************************************/
    int         get_balance     (const typename BSTree<T>::BinTreeNode* tree) const;
    /********************************************************************************//*!
    @brief      Rotates a subtree on the left.

    @param      tree
        The tree to balance from.

    @returns    The balanced tree.
    *//*********************************************************************************/
    BinTree     rotate_left     (BinTree tree);
    /********************************************************************************//*!
    @brief      Rotates a subtree on the right.

    @param      tree
        The tree to balance from.

    @returns    The balanced tree.
    *//*********************************************************************************/
    BinTree     rotate_right    (BinTree tree);
};

#include "AVLTree.hpp"

#endif
