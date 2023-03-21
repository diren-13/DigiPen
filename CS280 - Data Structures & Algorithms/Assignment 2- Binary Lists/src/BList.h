/************************************************************************************//*!
 \file    BList.h
 \author  Diren D Bharwani, diren.dbharwani, 390002520
 \par     email: diren.dbharwani\@digipen.edu
 \date    Feb 2, 2022
 \brief   Contains the interface for the BList class, along with all its helper objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef BLIST_H
#define BLIST_H

// Standard Libraries
#include <string>

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
 @brief     Exception Class for BList.
*//*************************************************************************************/
class BListException : public std::exception
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      The exception type.
    *//*********************************************************************************/
    enum BLIST_EXCEPTION 
    {
        E_NO_MEMORY, 
        E_BAD_INDEX, 
        E_DATA_ERROR
    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Constructor for BListException

    @param      ErrCode
        The error code for the exception.
    @param      Description
        THe description of the exception.
    *//*********************************************************************************/
    BListException(int ErrCode, const std::string& Description) 
    : m_ErrCode     (ErrCode)
    , m_Description (Description) 
    {};
    /********************************************************************************//*!
    @brief      Destructor for BListException
    *//*********************************************************************************/
    virtual ~BListException() {}

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Getter for the error code.

    @return     The error code.
    *//*********************************************************************************/
    virtual inline int code() const { return m_ErrCode; }
    /********************************************************************************//*!
    @brief      Getter for the error message.

    @return     The description of the exception.
    *//*********************************************************************************/
    virtual inline const char *what() const throw() { return m_Description.c_str(); }

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    int         m_ErrCode;          //!< One of E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR
    std::string m_Description;      //!< Description of the exception
};

/************************************************************************************//*!
 @brief     Statistics for a BList.
*//*************************************************************************************/
struct BListStats
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    size_t  NodeSize;   //!< Size of a node (via sizeof)
    int     NodeCount;  //!< Number of nodes in the list
    int     ArraySize;  //!< Max number of items in each node
    int     ItemCount;  //!< Number of items in the entire list

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Default Constructor for BListStats.
    *//*********************************************************************************/
    BListStats() 
    : NodeSize  (0)
    , NodeCount (0)
    , ArraySize (0)
    , ItemCount (0)
    {};

    /********************************************************************************//*!
    @brief      Constructor for BListStats.

    @param      nodeSize
        Size of a node.
    @param      nodeCount
        Number of nodes in the BList.
    @param      arraySize
        Number of elements in each node.
    @param      numElems
        Number of elements in the list.
    *//*********************************************************************************/
    BListStats(size_t nodeSize, int nodeCount, int arraySize, int numElems) 
    : NodeSize  (nodeSize)
    , NodeCount (nodeCount)
    , ArraySize (arraySize)
    , ItemCount (numElems)
    {};
};  

/************************************************************************************//*!
 @brief     A BList.

 @tparam    T
    The type of each element in the BList.
 @tparam    Size
    The Size of the BList on initialisation. Defaults to 1.
*//*************************************************************************************/
template <typename T, unsigned int Size = 1>
class BList
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief     A Node for the BList.
    *//*********************************************************************************/
    struct BNode
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        BNode*  next;           //!< pointer to next BNode
        BNode*  prev;           //!< pointer to previous BNode
        int     count;          //!< number of items currently in the node
        T       values[Size];   //!< array of items in the node

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        @brief     Default Constructor for a BNode.
        *//*****************************************************************************/
        BNode() 
        : next  (0)
        , prev  (0)
        , count (0)
        {}
    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Default Constructor for a BList.

    @throws     BListException::E_NO_MEMORY, if there is no physical memory left for
                allocation.
    *//*********************************************************************************/
    BList();
    /********************************************************************************//*!
    @brief      Copy Constructor for a BList. Performs a deep copy.

    @param      rhs
        A BList to copy the data from.

    @throws     BListException::E_NO_MEMORY, if there is no physical memory left for
                allocation.
    *//*********************************************************************************/
    BList(const BList &rhs);
    /********************************************************************************//*!
    @brief      Destructor for a BList.
    *//*********************************************************************************/
    ~BList();

    /*---------------------------------------------------------------------------------*/
    /* Operator Overloads                                                              */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Copy Assignment for a BList. Performs a deep copy.

    @param      rhs
        A BList to copy the data from.

    @return     Reference to this BList.

    @throws     BListException::E_NO_MEMORY, if there is no physical memory left for
                allocation.
    *//*********************************************************************************/
    BList& operator=(const BList &rhs);
    /********************************************************************************//*!
    @brief      Gets an element from the BList.

    @param      index
        The index of an element in the BList.

    @return     A reference to the element in the BList.

    @throws     BListException::E_BAD_INDEX if the index specified was out of range.
    *//*********************************************************************************/
    T& operator[](int index);
    /********************************************************************************//*!
    @brief      Gets an element from the BList.

    @param      index
        The index of an element in the BList.

    @return     A read-only reference to the element in the BList.

    @throws     BListException::E_BAD_INDEX if the index specified was out of range.
    *//*********************************************************************************/
    const T& operator[](int index) const;

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Gets a pointer to the head of the BList.

    @returns    A read-only pointer to the head of the BList.
    *//*********************************************************************************/
    const BNode* GetHead() const;
    /********************************************************************************//*!
    @brief      Gets the statistics of the BList.

    @returns    The statistics of the BList.
    *//*********************************************************************************/
    BListStats GetStats() const;
    /********************************************************************************//*!
    @brief      Gets the size of the BList.
    
    @returns    The total number of elements in the BList.
    *//*********************************************************************************/
    size_t size() const;
    /********************************************************************************//*!
    @brief      Gets the size of a node in the BList.

    @returns    The size of a node in the BList.
    *//*********************************************************************************/
    static size_t nodesize();

    /*---------------------------------------------------------------------------------*/
    /* Function Memebrs                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Copies an element into the back of the BList.
                Does not sort the arrays in the nodes.

    @param      value
        The element to put into the BList.
    *//*********************************************************************************/
    void push_back(const T& value);
    /********************************************************************************//*!
    @brief      Copies an element into the front of the BList.
                Does not sort the arrays in the nodes.

    @param      value
        The element to put into the BList.
    *//*********************************************************************************/
    void push_front(const T& value);
    /********************************************************************************//*!
    @brief      Copies an element into the BList.
                Sorts the arrays in the nodes.

    @param      value
        The element to put into the BList.
    *//*********************************************************************************/
    void insert(const T& value);
    /********************************************************************************//*!
    @brief      Removes an element from the BList.

    @param      index
        The index of the element to remove.

    @throws     BListException::E_BAD_INDEX if the index specified was out of range.
    *//*********************************************************************************/
    void remove(int index);
    /********************************************************************************//*!
    @brief      Removes an element from the BList based on its value.

    @param      value
        The value of the element to remove.
    *//*********************************************************************************/
    void remove_by_value(const T& value);
    /********************************************************************************//*!
    @brief      Searches for a value in the BList.

    @param      value
        The element to find in the BList.
    
    @returns    The index where the value is stored. Otherwise, returns -1 if value is
                not found.
    *//*********************************************************************************/
    int find(const T& value) const;
    /********************************************************************************//*!
    @brief      Deletes all nodes in the BList.
    *//*********************************************************************************/
    void clear();
    
private:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      The state of the left and right nodes when using the insert method.
    *//*********************************************************************************/
    enum class InsertionState
    {
        L_NFULL_R_NFULL,    //!< both left and right are not full
        L_NFULL_R_FULL,     //!< left is not full but right is full
        L_FULL_R_NFULL,     //!< left is full but right is not full 
        L_FULL_R_FULL,      //!< both left and right are full
    };

    /*---------------------------------------------------------------------------------*/
    /* Data Memebrs                                                                    */
    /*---------------------------------------------------------------------------------*/
    BNode*      head;  //!< points to the first node
    BNode*      tail;  //!< points to the last node

    BListStats  stats;  //!< stats for the BList

    /*---------------------------------------------------------------------------------*/
    /* Function Memebrs                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Allocates an empty node at the front of the BList.
    
    @returns    The allocated node. Actually the head.
    *//*********************************************************************************/
    BNode* allocateNodeInFront();
    /********************************************************************************//*!
    @brief      Allocates an empty node at the back of the BList.
    
    @returns    The allocated node. Actually the tail.
    *//*********************************************************************************/
    BNode* allocateNodeAtBack();
    /********************************************************************************//*!
    @brief      Splits a node into two.
    
    @param      node
        The node to split.
    *//*********************************************************************************/
    void splitNode(BNode* node);
    /********************************************************************************//*!
    @brief      Removes a node from the BList.
    
    @param      node
        The node to remove.
    *//*********************************************************************************/
    void removeNode(BNode* node);
    /********************************************************************************//*!
    @brief      Inserts a value into a node.
    
    @param      value
        The value to insert.
    @param      node
        The node to insert the value in.
    *//*********************************************************************************/
    void insertIntoNode(const T& value, BNode* node);
    /********************************************************************************//*!
    @brief      Inserts a value into the head node. If the head node is full, it will be
                split.
    
    @param      value
        The value to insert.
    *//*********************************************************************************/
    void insertAtHead(const T& value);
    /********************************************************************************//*!
    @brief      Inserts a value into the tail node. If the head node is full, it will be
                split.
    
    @param      value
        The value to insert.
    *//*********************************************************************************/
    void insertAtTail(const T& value);
    /********************************************************************************//*!
    @brief      Inserts a value into a node right after it has been split.
    
    @param      value
        The value to insert.
    @param      left
        The node which was split.
    @param      right
        The new node.
    *//*********************************************************************************/
    void insertAfterSplit(const T& value, BNode* left, BNode* right);
    /********************************************************************************//*!
    @brief      Removes an element from a node.
    
    @param      pos
        The position in the node to remove the element at.
    @param      node
        The node to remove the element at.
    *//*********************************************************************************/
    void removeElement(int pos, BNode* node);
    /********************************************************************************//*!
    @brief      Checks if a node is full.
    
    @param      node
        The node to check.
    *//*********************************************************************************/
    bool isNodeFull(const BNode* node) const;
    /********************************************************************************//*!
    @brief      Checks if a value is in the range of the node's elements.
    
    @param      value
        The value to check.
    @param      node
        The node to check in.

    @returns    True if the value is in the range.
    *//*********************************************************************************/
    bool inRange(const T& value, const BNode* node) const;
    /********************************************************************************//*!
    @brief      Checks if a value is in the range of two node's elements.
    
    @param      value
        The value to check.
    @param      left
        The start node to check in.
    @param      right
        The end node to check in.

    @returns    True if the value is in the range.
    *//*********************************************************************************/
    bool inRange(const T& value, const BNode* left, const BNode* right) const;
};

#include "BList.hpp"

#endif // BLIST_H
