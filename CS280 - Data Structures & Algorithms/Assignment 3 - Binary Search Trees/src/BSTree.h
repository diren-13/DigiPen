/************************************************************************************//*!
 @file    BSTree.h
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the interface for the BSTException, and BSTree objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef BSTREE_H
#define BSTREE_H

// Standard Libraries
#include <string>
#include <stdexcept>
// Project Headers
#include "ObjectAllocator.h"

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Exception Class for the AVL/BST classes.
*//*************************************************************************************/
class BSTException : public std::exception
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      The exception type.
    *//*********************************************************************************/
    enum BST_EXCEPTION
    {
        E_NO_MEMORY
    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Constructor for BSTException

    @param      ErrCode
        The error code for the exception.
    @param      Message
        THe description of the exception.
    *//*********************************************************************************/
    BSTException(int ErrCode, const std::string& Message) 
    : error_code_(ErrCode)
    , message_(Message) 
    {};
    /********************************************************************************//*!
    @brief      Destructor for BSTException
    *//*********************************************************************************/
    virtual ~BSTException() {}

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Getter for the error code.

    @return     The error code.
    *//*********************************************************************************/
    virtual inline int          code()  const           { return error_code_; }
    /********************************************************************************//*!
    @brief      Getter for the error message.

    @return     The description of the exception.
    *//*********************************************************************************/
    virtual inline const char*  what()  const throw()   { return message_.c_str(); }

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    int         error_code_;    //!< The code of the exception
    std::string message_;       //!< Readable message text
};

/************************************************************************************//*!
 @brief     Encapsulates a Binary Search Tree.

 @tparam    T
    The type of an element in the Binary Search Tree.
*//*************************************************************************************/
template <typename T>
class BSTree
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Encapsulates a node in the binary tree.
    *//*********************************************************************************/
    struct BinTreeNode
    {
    public:
        BinTreeNode*    left;               //!< The left child
        BinTreeNode*    right;              //!< The right child
        T               data;               //!< The data
        int             balance_factor;     //!< optional for efficient balancing
        unsigned        count;              //!< nodes in this subtree for efficient indexing

        /****************************************************************************//*!
        @brief      Default Constructor for BinTreeNode.
        *//*****************************************************************************/
        BinTreeNode() 
        : left          (0)
        , right         (0)
        , data          (0)
        , balance_factor(0)
        , count         (1)
        {};
        /****************************************************************************//*!
        @brief      Default Constructor for BinTreeNode.

        @param      value
            The value to construct as the BinTreeNode.
        *//*****************************************************************************/
        BinTreeNode(const T& value) 
        : left          (0)
        , right         (0)
        , data          (value)
        , balance_factor(0)
        , count         (1)
        {};
    };

    /*---------------------------------------------------------------------------------*/
    /* Type  Aliases                                                                   */
    /*---------------------------------------------------------------------------------*/
    using BinTree = BinTreeNode*;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Default Constructor for a BSTree.

    @param      oa
        A pointer to the object allocator to use for memory management.
    @param      ShareOA
        If the allocator is shared with others
    *//*********************************************************************************/
    BSTree(ObjectAllocator* oa = nullptr, bool ShareOA = false);
    /********************************************************************************//*!
    @brief      Copy Constructor for a BSTree.

    @param      rhs
        A BSTree to copy data from.
    *//*********************************************************************************/
    BSTree(const BSTree& rhs);
    /********************************************************************************//*!
    @brief      Destructor for a BSTree.
    *//*********************************************************************************/
    virtual ~BSTree();

    /*---------------------------------------------------------------------------------*/
    /* Operator Overloads                                                              */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Copy Assignment for a BSTree.

    @param      rhs
        A BSTree to copy data from.

    @returns    A reference to the copied tree.
    *//*********************************************************************************/
    BSTree&             operator=   (const BSTree& rhs);
    /********************************************************************************//*!
    @brief      Index access operator for the BSTree.

    @param      index
        The index of the node.

    @returns    A node at the index.
    *//*********************************************************************************/
    const BinTreeNode*  operator[]  (int index)             const;

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Gets the size of the tree.

    @returns    The number of nodes in the tree starting from the root.
    *//*********************************************************************************/
    unsigned int    size    ()  const;
    /********************************************************************************//*!
    @brief      Gets the height of the tree.

    @returns    The height of the tree starting from the root.
    *//*********************************************************************************/
    int             height  ()  const;
    /********************************************************************************//*!
    @brief      Gets the root node.

    @returns    A pointer to the root node.
    *//*********************************************************************************/
    BinTree         root    ()  const;
    /********************************************************************************//*!
    @brief      Checks if the tree is empty.

    @returns    True if the tree is empty.
    *//*********************************************************************************/
    bool            empty   ()  const;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Inserts an element into the tree.

    @param      value
        The value of the element to insert into the tree.
    *//*********************************************************************************/
    virtual void    insert  (const T& value);
    /********************************************************************************//*!
    @brief      Removes an element from the tree.

    @param      value
        The value of the element to remove from the tree.
    *//*********************************************************************************/
    virtual void    remove  (const T& value);
    /********************************************************************************//*!
    @brief      Removes all elements from the tree.
    *//*********************************************************************************/
    void            clear   ();
    /********************************************************************************//*!
    @brief      Finds an element in the tree

    @param      value
        The value to search for.
    @param      compares
        The number of times the find function is recursively invoked.

    @returns    True if the value is found.
    *//*********************************************************************************/
    bool            find    (const T& value, unsigned& compares)    const;

  protected:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    ObjectAllocator*    allocator;
    bool                shareOA;
    BinTree             rootNode;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Getter for the root.

    @returns    A reference to the root node.
    *//*********************************************************************************/
    BinTree&            get_root        ();
    /********************************************************************************//*!
    @brief      Getter for the allocator.

    @returns    A reference to the root node.
    *//*********************************************************************************/
    ObjectAllocator*&   get_allocator   ();
    /********************************************************************************//*!
    @brief      Getter for the root.

    @returns    A reference to the root node.
    *//*********************************************************************************/
    bool&               get_shared_oa   ();
    /********************************************************************************//*!
    @brief      Creates a node.

    @param      value
        The value of the node to create.

    @returns    The newly created node.
    *//*********************************************************************************/
    BinTree         make_node       (const T& value)                    const;
    /********************************************************************************//*!
    @brief      Destroys a node.

    @param      node
        The node to destroy.
    *//*********************************************************************************/
    void            free_node       (BinTree node);
    /********************************************************************************//*!
    @brief      Gets the height of a sub tree.

    @param      tree
        The root node of the subtree.

    @returns    The height of the sub tree.
    *//*********************************************************************************/
    int             tree_height     (BinTree tree)                          const;
    /********************************************************************************//*!
    @brief      Finds the predecessor of a node.

    @param      tree
        The node to find the predecessor of.
    @param      predecessor
        The predecessor of the node.
    *//*********************************************************************************/
    void            find_predecessor  (BinTree tree, BinTree &predecessor)      const;
    /********************************************************************************//*!
    @brief      Inserts a node with a new value into the tree.

    @param      tree
        The root node of the subtree.
    @param      value
        The new value to insert.

    @returns    The root node of the subtree.
    *//*********************************************************************************/
    virtual BinTree insert_node     (BinTree& tree, const T& value);
    /********************************************************************************//*!
    @brief      Removes a node with the specified from the tree.

    @param      tree
        The root node of the subtree.
    @param      value
        The value to search for to remove.

    @returns    The root node of the subtree.fr
    *//*********************************************************************************/
    virtual BinTree remove_node     (BinTree& tree, const T& value);
    
  private:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Recursively copies nodes.

    @param      dest
        The root node of the subtree.
    @param      src
        The value to search for to remove.
    *//*********************************************************************************/
    void                recursive_copy  (BinTree& dest, const BinTree src);
    /********************************************************************************//*!
    @brief      Searches for a node with a matching value

    @param      tree
        The root node of the subtree to search in.
    @param      value
        The value to search for.
    @param      numInvokes
        The number of times this function has been called.

    @returns    True if the value is been found.
    *//*********************************************************************************/
    bool                find_value      (const BinTree tree, const T& value, unsigned& numInvokes)  const;
    /********************************************************************************//*!
    @brief      Searches for a node with a a given index.

    @param      tree
        The root node of the subtree.
    @param      index
        The index to search for.

    @returns    The node that has been found.
    *//*********************************************************************************/
    const BinTreeNode*  find_node       (const BinTree tree, int index)                             const;
    /********************************************************************************//*!
    @brief      Recursively destroys nodes.

    @param      tree
        The root node of the tree to destroy from.
    *//*********************************************************************************/
    void                recursive_free  (BinTree tree);
};

#include "BSTree.hpp"

#endif
