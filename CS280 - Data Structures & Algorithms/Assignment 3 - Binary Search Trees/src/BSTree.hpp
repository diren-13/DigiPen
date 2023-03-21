/************************************************************************************//*!
 @file    BSTree.cpp
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the implementation for the BSTree object.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "BSTree.h"
// Standard Libraries
#include <iostream>     // For debugging only
#include <exception>

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Default Constructor for a BSTree.
 
 @param      oa
     A pointer to the object allocator to use for memory management.
 @param      ShareOA
    If the allocator is shared with others
*//*************************************************************************************/
template <typename T>
BSTree<T>::BSTree(ObjectAllocator* oa, bool ShareOA)
: allocator { oa }
, shareOA   { ShareOA }
, rootNode  { nullptr }
{
    if (!allocator)
    {
        OAConfig config{true};
        allocator = new ObjectAllocator(sizeof(BinTreeNode), config);
    }
}
/************************************************************************************//*!
 @brief      Copy Constructor for a BSTree.

 @param      rhs
    A BSTree to copy data from.
*//*************************************************************************************/
template <typename T>
BSTree<T>::BSTree(const BSTree& rhs)
: shareOA   { rhs.shareOA }
, rootNode  { nullptr }
{
    if (shareOA)
    {
        allocator = rhs.allocator;
    }
    else
    {
        OAConfig config{true};
        allocator = new ObjectAllocator(sizeof(BinTreeNode), config);
    }

    // Traverse current tree in pre-order
    recursive_copy(rootNode, rhs.rootNode);
}
/************************************************************************************//*!
 @brief      Destructor for a BSTree.
*//*************************************************************************************/
template <typename T>
BSTree<T>::~BSTree()
{
    clear();
    delete allocator;
    allocator = nullptr;
}

/*-------------------------------------------------------------------------------------*/
/* Operator Overloads                                                                  */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Copy Assignment for a BSTree.
 
 @param      rhs
     A BSTree to copy data from.
 
 @returns    A reference to the copied tree.
*//*************************************************************************************/
template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs)
{
    if (shareOA)
    {
        delete allocator;
        allocator = rhs.allocator;
    }

    clear();
    recursive_copy(rootNode, rhs.rootNode);
    return *this;
}
/************************************************************************************//*!
 @brief      Index access operator for the BSTree.

 @param      index
    The index of the node.

 @returns    A node at the index.
*//*************************************************************************************/
template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const
{
    return find_node(rootNode, index);
}

/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Gets the size of the tree.
 
 @returns    The number of nodes in the tree starting from the root.
*//*************************************************************************************/
template <typename T>
unsigned int BSTree<T>::size() const
{
    return rootNode ? rootNode->count : 0;
}
/************************************************************************************//*!
 @brief      Gets the height of the tree.

 @returns    The height of the tree starting from the root.
*//*************************************************************************************/
template <typename T>
int BSTree<T>::height() const
{
    return tree_height(rootNode);
}
/************************************************************************************//*!
 @brief      Gets the root node.

 @returns    A pointer to the root node.
*//*************************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
    return rootNode;
}
/************************************************************************************//*!
 @brief      Checks if the tree is empty.

 @returns    True if the tree is empty.
*//*************************************************************************************/
template <typename T>
bool BSTree<T>::empty() const
{
    return rootNode == nullptr;
}
/************************************************************************************//*!
 @brief      Inserts an element into the tree.

 @param      value
    The value of the element to insert into the tree.
*//*************************************************************************************/
template <typename T>
void BSTree<T>::insert(const T& value)
{
    rootNode = insert_node(rootNode, value);
}
/************************************************************************************//*!
 @brief      Removes an element from the tree.

 @param      value
    The value of the element to remove from the tree.
*//*************************************************************************************/
template <typename T>
void BSTree<T>::remove(const T&  value)
{
    rootNode = remove_node(rootNode, value);
}
/************************************************************************************//*!
 @brief      Removes all elements from the tree.
*//*************************************************************************************/
template<typename T>
void BSTree<T>::clear()
{
    recursive_free(rootNode);
    rootNode = nullptr;
}
/************************************************************************************//*!
 @brief      Finds an element in the tree

 @param      value
    The value to search for.
 @param      compares
    The number of times the find function is recursively invoked.

 @returns    True if the value is found.
*//*************************************************************************************/
template <typename T>
bool BSTree<T>::find(const T& value, unsigned& compares) const
{
    compares = 0;
    return find_value(rootNode, value, compares);
}

/*-------------------------------------------------------------------------------------*/
/* Protected Function Members                                                          */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Getter for the root.

 @returns    A reference to the root node.
*//*************************************************************************************/
template <typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root()
{
    return rootNode;
}
/************************************************************************************//*!
 @brief      Getter for the allocator.

 @returns    A reference to the root node.
*//*************************************************************************************/
template <typename T>
ObjectAllocator*& BSTree<T>::get_allocator()
{
    return allocator;
}
/************************************************************************************//*!
 @brief      Getter for the root.

 @returns    A reference to the root node.
*//*************************************************************************************/
template <typename T>
bool& BSTree<T>::get_shared_oa()
{
    return shareOA;
}
/************************************************************************************//*!
 @brief      Creates a node.

 @param      value
    The value of the node to create.

 @returns    The newly created node.
*//*************************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T& value) const
{
    try
    {
        BinTree newNode = static_cast<BinTree>(allocator->Allocate());
        newNode = new (newNode) BinTreeNode{value};
        return newNode;
    }
    catch (...)
    {
        throw BSTException{BSTException::BST_EXCEPTION::E_NO_MEMORY, "No memory to allocate a node!"};
    }
}
/************************************************************************************//*!
 @brief      Destroys a node.

 @param      node
    The node to destroy.
*//*************************************************************************************/
template <typename T>
void BSTree<T>::free_node(BinTree node)
{
    try
    {
        allocator->Free(node);
    }
    catch(...)
    {
        throw BSTException{BSTException::BST_EXCEPTION::E_NO_MEMORY, "Error with freeing a node!"};
    }
}
/************************************************************************************//*!
 @brief      Gets the height of a sub tree.

 @param      tree
    The root node of the subtree.

 @returns    The height of the sub tree.
*//*************************************************************************************/
template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
    return (tree) ? std::max(tree_height(tree->left) + 1, tree_height(tree->right) + 1) : -1;
}
/************************************************************************************//*!
 @brief      Finds the predecessor of a node.

 @param      tree
    The node to find the predecessor of.
 @param      predecessor
    The predecessor of the node.
*//*************************************************************************************/
template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree& predecessor) const
{
    predecessor = tree;
    while (predecessor and predecessor->right)
    {
        predecessor = predecessor->right;
    }
}
/************************************************************************************//*!
 @brief      Inserts a node with a new value into the tree.

 @param      tree
    The root node of the subtree.
 @param      value
    The new value to insert.

 @returns    The root node of the subtree.
*//*************************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::insert_node(BinTree& tree, const T& value)
{
    if (tree == nullptr)
        return make_node(value);

    // Smaller values traverse to the left
    if (value < tree->data)
    {
        tree->left = insert_node(tree->left, value);
    }

    // Larger values traverse to the right
    if (value > tree->data)
    {
        tree->right = insert_node(tree->right, value);
    }

    ++(tree->count);
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
typename BSTree<T>::BinTree BSTree<T>::remove_node(BinTree& tree, const T& value)
{
    if (tree == nullptr)
        return tree;

    // Smaller values traverse to the left
    if (value < tree->data)
    {
        tree->left = remove_node(tree->left, value);
        --(tree->count);
        return tree;
    }
    if (value > tree->data)
    {
        tree->right = remove_node(tree->right, value);
        --(tree->count);
        return tree;
    }

    --(tree->count);

    // If one or no child
    if (tree->left == nullptr || tree->right == nullptr)
    {
        BinTree temp = tree->left ? tree->left : tree->right;

        if (!temp)
        {
            free_node(tree);
            return nullptr;
        }
        else
        {
            free_node(tree);
            return temp;
        }
    }
    // Both left and right child exist
    else
    {
        BinTree predecessor = nullptr;
        find_predecessor(tree->left, predecessor);
        tree->data = predecessor->data;
        tree->left= remove_node(tree->left, predecessor->data);
    }

    return tree;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief      Recursively copies nodes.

 @param      dest
    The root node of the subtree.
 @param      src
    The value to search for to remove.
*//*************************************************************************************/
template <typename T>
void BSTree<T>::recursive_copy(BinTree& dest, const BinTree src)
{
    if (src == nullptr)
        return;

    dest = make_node(src->data);
    dest->count = src->count;
    
    recursive_copy(dest->left, src->left);
    recursive_copy(dest->right, src->right);
}
/************************************************************************************//*!
 @brief      Searches for a node with a matching value

 @param      tree
    The root node of the subtree to search in.
 @param      value
    The value to search for.
 @param      numInvokes
    The number of times this function has been called.

 @returns    True if the value is been found.
*//*************************************************************************************/
template <typename T>
bool BSTree<T>::find_value(const BinTree tree, const T& value, unsigned& numInvokes) const
{
    ++numInvokes;

    if (tree == nullptr)
        return false;

    if (value < tree->data)
        return find_value(tree->left, value, numInvokes);

    if (value > tree->data)
        return find_value(tree->right, value, numInvokes);

    return true;
}
/************************************************************************************//*!
 @brief      Searches for a node with a a given index.

 @param      tree
    The root node of the subtree.
 @param      index
    The index to search for.

 @returns    The node that has been found.
*//*************************************************************************************/
template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::find_node(const BinTree tree, int index) const
{
    if (tree == nullptr)
        return nullptr;

    const int L = tree->left ? tree->left->count : 0;

    if (index < L)
        return find_node(tree->left, index);

    if (index > L)
        return find_node(tree->right, index - L - 1);

    return tree;
}
/************************************************************************************//*!
 @brief      Recursively destroys nodes.

 @param      tree
    The root node of the tree to destroy from.
*//*************************************************************************************/
template <typename T>
void BSTree<T>::recursive_free(BinTree tree)
{
    if (tree == nullptr)
        return;

    recursive_free(tree->left);
    recursive_free(tree->right);

    free_node(tree);
}