/************************************************************************************//*!
\file           BList.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Feb 2, 2022
\brief          Contains the implementation of the BList class.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "BList.h"
// Standard Libraries
#include <iostream>     // For debugging only
#include <exception>
#include <cstring>

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Default Constructor for a BList.

 @throws    BListException::E_NO_MEMORY, if there is no physical memory left for
            allocation.
*//*************************************************************************************/
template <typename T, unsigned int Size>
BList<T, Size>::BList()
: head  { nullptr }
, tail  { nullptr }
, stats {}
{
    try
    {
        // Allocate a single node.
        head = tail = new BNode;
        head->count = 0;
        ++stats.NodeCount;
        
        stats.NodeSize    = sizeof(BNode);
        stats.ArraySize   = static_cast<int>(Size);
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
/************************************************************************************//*!
 @brief     Copy Constructor for a BList. Performs a deep copy.

 @param     rhs
    A BList to copy the data from.

 @throws    BListException::E_NO_MEMORY, if there is no physical memory left for
            allocation.
*//*************************************************************************************/
template <typename T, unsigned int Size>
BList<T, Size>::BList(const BList<T, Size>& rhs)
: head  { nullptr }
, tail  { nullptr }
, stats {}
{
    try
    {
        // Allocate the first node
        head = tail = new BNode;
        head->count = rhs.head->count;
        memcpy(head->values, rhs.head->values, sizeof(T) * rhs.head->count);

        // Allocate remaining nodes and reassign
        BNode* rNode = rhs.head->next;
        while (rNode)
        {
            BNode* node = allocateNodeAtBack();
            node->count = rNode->count;
            memcpy(node->values, rNode->values, sizeof(T) * rhs.head->count);

            rNode = rNode->next;
        }

        stats.NodeSize  = rhs.nodesize();
        stats.ItemCount = rhs.stats.ItemCount;
        stats.ArraySize = Size;
        stats.NodeCount = rhs.stats.NodeCount;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
/************************************************************************************//*!
 @brief      Destructor for a BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
BList<T, Size>::~BList()
{
    clear();
}
/*-------------------------------------------------------------------------------------*/
/* Operator Overloads                                                                  */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Copy Assignment for a BList. Performs a deep copy.

 @param     rhs
    A BList to copy the data from.

 @return    Reference to this BList.

 @throws    BListException::E_NO_MEMORY, if there is no physical memory left for
            allocation.
*//*************************************************************************************/
template <typename T, unsigned int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList<T, Size>& rhs)
{
    // Clear the BList and reallocate to copy data from rhs
    clear();

    // Copy stats over
    try
    {
        // Allocate the first node
        head = tail = new BNode;
        head->count = rhs.head->count;
        memcpy(head->values, rhs.head->values, sizeof(T) * rhs.head->count);

        // Allocate remaining nodes and reassign
        BNode* rNode = rhs.head->next;
        while (rNode)
        {
            BNode* node = allocateNodeAtBack();
            node->count = rNode->count;
            memcpy(node->values, rNode->values, sizeof(T) * rhs.head->count);

            rNode = rNode->next;
        }

        stats.NodeSize  = rhs.nodesize();
        stats.ItemCount = rhs.stats.ItemCount;
        stats.ArraySize = Size;
        stats.NodeCount = rhs.stats.NodeCount;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }

    return *this;
}
/************************************************************************************//*!
 @brief     Gets an element from the BList.

 @param     index
    The index of an element in the BList.

 @return    A reference to the element in the BList.

 @throws    BListException::E_BAD_INDEX if the index specified was out of range.
*//*************************************************************************************/
template <typename T, unsigned int Size>
T& BList<T, Size>::operator[](int index)
{
    if (index < stats.ItemCount)
    {
        int counter = -1;

        BNode* node = head;
        while(node)
        {
            for (int i = 0; i < node->count; ++i)
            {
                ++counter;
                if (counter == index)
                    return node->values[i];
            }

            node = node->next;
        }
    }

    throw BListException{BListException::E_BAD_INDEX, "Index out of range."};
}
/************************************************************************************//*!
 @brief     Gets an element from the BList.

 @param     index
    The index of an element in the BList.

 @return    A read-only reference to the element in the BList.

 @throws    BListException::E_BAD_INDEX if the index specified was out of range.
*//*************************************************************************************/
template <typename T, unsigned int Size>
const T& BList<T, Size>::operator[](int index) const
{
    if (index < stats.ItemCount)
    {
        int counter = -1;

        BNode* node = head;
        while(node)
        {
            for (int i = 0; i < node->count; ++i)
            {
                ++counter;
                if (counter == index)
                    return node->values[i];
            }

            node = node->next;
        }
    }

    throw BListException{BListException::E_BAD_INDEX, "Index out of range."};
}
/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Gets a pointer to the head of the BList.

 @return    A read-only pointer to the head of the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
    return head;
}
/************************************************************************************//*!
 @brief     Gets the statistics of the BList.

 @return    The statistics of the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
BListStats BList<T,Size>::GetStats() const
{
    return stats;
}
/************************************************************************************//*!
 @brief     Gets the size of the BList.

 @return    The total number of elements in the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
size_t BList<T, Size>::size() const
{
    return stats.ItemCount;
}
/************************************************************************************//*!
 @brief     Gets the size of a node in the BList.

 @return    The size of a node in the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
size_t BList<T, Size>::nodesize(void)
{
    return sizeof(BNode);
}
/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Copies an element into the back of the BList.
            Does not sort the arrays in the nodes.

 @param     value
    The element to put into the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::push_back(const T& value)
{
    if (tail->count == Size)
    {
        allocateNodeAtBack();
    }

    // Back deals with tail node
    tail->values[tail->count] = value;

    // Increment stats
    ++tail->count;
    ++stats.ItemCount;
}
/************************************************************************************//*!
 @brief     Copies an element into the front of the BList.
            Does not sort the arrays in the nodes.

 @param     value
    The element to put into the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::push_front(const T& value)
{
    if (head->count == Size)
    {
        allocateNodeInFront();
    }

    // Front deals with head node
    for (int i = head->count; i > 0; --i)
    {
        head->values[i] = head->values[i - 1];
    }
    head->values[0] = value;

    // Increment stats
    ++head->count;
    ++stats.ItemCount;
}
/************************************************************************************//*!
 @brief     Copies an element into the BList.
            Sorts the arrays in the nodes.

 @param     value
    The element to put into the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::insert(const T& value)
{
    // If no values initialised, just insert.
    if (stats.ItemCount == 0)
    {
        head->values[0] = value;

        ++head->count;
        ++stats.ItemCount;

        return;
    }

    BNode* node = head;

    while (node)
    {
        BNode* left = node;
        BNode* right = node->next;

        // If head
        if (left == head && value < *(left->values))
        {
            insertAtHead(value);
            return;
        }
        
        // If tail
        if (left == tail)
        {
            insertAtTail(value);
            return;
        }

        // Look for range where value belongs
        // If outside of left and right range, or is in right, move to next node.
        if (!inRange(value, left, right) || inRange(value, right))
        {
            node = node->next;
            continue;
        }

        // Set the state of the left and right node
        const bool IsLeftFull  = isNodeFull(left);
        const bool IsRightFull = isNodeFull(right);

        const InsertionState LRState = IsLeftFull ? (IsRightFull ? InsertionState::L_FULL_R_FULL  : InsertionState::L_FULL_R_NFULL)
                                                  : (IsRightFull ? InsertionState::L_NFULL_R_FULL : InsertionState::L_NFULL_R_NFULL);

        switch (LRState)
        {
            // Both left and right are not full
            case InsertionState::L_NFULL_R_NFULL:
            {
                // Prefer left
                insertIntoNode(value, left);
                return;
            }
            // Both left and right are full
            case InsertionState::L_FULL_R_FULL:
            {
                splitNode(left);
                right = left->next;
                insertAfterSplit(value, left, right);
                return;
            }
            // Left not full but right is full
            case InsertionState::L_NFULL_R_FULL:
            {
                insertIntoNode(value, left);
                return;
            }
            // Left is full but right is not full
            case InsertionState::L_FULL_R_NFULL:
            {
                if (inRange(value, left))
                {
                    splitNode(left);
                    right = left->next;
                    insertAfterSplit(value, left, right);
                }
                else
                {
                    insertIntoNode(value, right);
                }
                return;
            }
            default: break;
        }

        node = node->next;
    }
}
/************************************************************************************//*!
 @brief     Removes an element from the BList.

 @param     index
    The index of the element to remove.

 @throws    BListException::E_BAD_INDEX if the index specified was out of range.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::remove(int index)
{
    if (index >= stats.ItemCount)
        throw BListException{BListException::E_BAD_INDEX, "Index out of range"};

    int counter = 0;

    BNode* node = head;
    while(node)
    {
        for (int i = 0; i < node->count; ++i)
        {
            if (counter == index)
            {
                removeElement(i, node);
                return;
            }
            ++counter;
        }
        node = node->next;
    }
}
/************************************************************************************//*!
 @brief     Removes an element from the BList based on its value.

 @param     value
    The value of the element to remove.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::remove_by_value(const T& value)
{
    BNode* node = head;
    while (node)
    {
        for (int i = 0; i < node->count; ++i)
        {
            if (node->values[i] == value)
            {
                removeElement(i, node);
                return;
            }
        }
        node = node->next;
    }
}
/************************************************************************************//*!
 @brief     Searches for a value in the BList.

 @param     value
    The element to find in the BList.
    
 @returns   The index where the value is stored. Otherwise, returns -1 if value is
            not found.
*//*************************************************************************************/
template <typename T, unsigned int Size>
int BList<T, Size>::find(const T& value) const
{
    int valuePos = -1;

    BNode* node = head;
    while(node)
    {
        for (int i = 0; i < node->count; ++i)
        {
            ++valuePos;
            if (node->values[i] == value)
                return valuePos;
        }

        node = node->next;
    }

    return -1;
}
/************************************************************************************//*!
 @brief     Deletes all nodes in the BList.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::clear()
{
    while (head)
    {
        BNode* next = head->next;
        delete head;
        head = next;
    }

    stats.NodeCount = 0;
    stats.ItemCount = 0;
} 


/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Allocates an empty node at the front of the BList.

 @returns   The allocated node. Actually the head.
*//*************************************************************************************/
template <typename T, unsigned int Size>
typename BList<T, Size>::BNode* BList<T, Size>::allocateNodeInFront()
{
    try
    {
        BNode* temp = head;
        head = new BNode;
        temp->prev = head;
        head->next = temp;

        head->count = 0;
        ++stats.NodeCount;

        return head;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
/************************************************************************************//*!
 @brief     Allocates an empty node at the back of the BList.

 @returns   The allocated node. Actually the tail.
*//*************************************************************************************/
template <typename T, unsigned int Size>
typename BList<T, Size>::BNode* BList<T, Size>::allocateNodeAtBack()
{
    try
    {
        BNode* temp = tail;
        tail = new BNode;
        temp->next = tail;
        tail->prev = temp;

        tail->count = 0;
        ++stats.NodeCount;

        return tail;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
/************************************************************************************//*!
 @brief     Splits a node into two.

 @param     node
    The node to split.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::splitNode(BNode* node)
{
    const unsigned int HalfSize = (Size > 1) ? Size >> 1 : 1;

    BNode* newNode = new BNode;

    // Split Data
    node->count = static_cast<int>(HalfSize);

    // Edge case where size is 1, so data cannot be split. 
    // An empty node is created instead.
    if (Size > 1)
    {
        const T* secondHalf = node->values + HalfSize;
        memcpy(newNode->values, secondHalf, sizeof(T) * HalfSize);
        newNode->count = static_cast<int>(HalfSize);
    }

    if (node != tail)
    {
        BNode* temp = node->next;
        temp->prev = newNode;
    }
    else
    {
        tail = newNode;
    }

    newNode->next = node->next;
    node->next = newNode;
    newNode->prev = node;

    ++stats.NodeCount;
}
/************************************************************************************//*!
 @brief     Removes a node from the BList.

 @param     node
    The node to remove.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::removeNode(BNode* node)
{
    BNode* next = (node == tail ? nullptr : node->next);
    BNode* prev = (node == head ? nullptr : node->prev);

    if (next)
    {
        next->prev = prev;
    }

    if (prev)
    {
        prev->next = next;
    }

    if (node == head)
    {
        head = next;
    }

    if (node == tail)
    {
        tail = prev;
    }

    delete node;
    --stats.NodeCount;
}
/************************************************************************************//*!
 @brief     Inserts a value into a node.

 @param     value
    The value to insert.
 @param node
    The node to insert the value in.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::insertIntoNode(const T& value, BNode* node)
{
    int insertIdx = 0;
    for (; insertIdx < node->count; ++insertIdx)
    {
        if (value < node->values[insertIdx])
        {
            for (int j = node->count; j > insertIdx; --j)
            {
                node->values[j] = node->values[j-1];
            }
            
            break;
        }
    }

    node->values[insertIdx] = value;

    ++node->count;
    ++stats.ItemCount;
}
/************************************************************************************//*!
 @brief     Inserts a value into the head node. If the head node is full, it will be
            split.

 @param     value
    The value to insert.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::insertAtHead(const T& value)
{
    if (isNodeFull(head))
    {
        splitNode(head);
        insertAfterSplit(value, head, head->next);
    }
    else
    {
        insertIntoNode(value, head);
    }
}
/************************************************************************************//*!
 @brief     Inserts a value into the tail node. If the head node is full, it will be
            split.

 @param     value
    The value to insert.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::insertAtTail(const T& value)
{
    if (isNodeFull(tail))
    {
        splitNode(tail);
        insertAfterSplit(value, tail->prev, tail);
    }
    else
    {
        insertIntoNode(value, tail);
    }
}
/************************************************************************************//*!
 @brief     Inserts a value into a node right after it has been split.

 @param     value
    The value to insert.
 @param     left
    The node which was split.
 @param     right
    The new node.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::insertAfterSplit(const T& value, BNode* left, BNode* right)
{
    // Edge case for size of 1
    if (Size > 1)
    {
        insertIntoNode(value, (value < right->values[0]) ? left : right);
    }
    else
    {
        T& lValue = *(left->values);
        T& rValue = *(right->values);
        if (value < lValue)
        {
            // Swap values
            rValue = lValue;
            lValue = value;
        }
        else
        {
            rValue = value;
        }

        ++right->count;
        ++stats.ItemCount;
    }
}
/************************************************************************************//*!
 @brief     Removes an element from a node.

 @param     pos
    The position in the node to remove the element at.
 @param     node
    The node to remove the element at.
*//*************************************************************************************/
template <typename T, unsigned int Size>
void BList<T, Size>::removeElement(int pos, BNode* node)
{
    const int PrevCount = node->count--;
    --stats.ItemCount;

    if (node->count == 0)
    {
        removeNode(node);
        return;
    }

    for (int i = pos; i < PrevCount; ++i)
    {
        node->values[i] = node->values[i + 1];
    }
}
/************************************************************************************//*!
 @brief     Checks if a node is full.

 @param     node
    The node to check.
*//*************************************************************************************/
template <typename T, unsigned int Size>
bool BList<T, Size>::isNodeFull(const BNode* node) const
{
    return node->count == static_cast<int>(Size);
}
/************************************************************************************//*!
 @brief     Checks if a value is in the range of the node's elements.

 @param     value
    The value to check.
 @param     node
    The node to check in.

 @return    True if the value is in the range.
*//*************************************************************************************/
template <typename T, unsigned int Size>
bool BList<T, Size>::inRange(const T& value, const BNode* node) const
{
    const T& Min = node->values[0];
    const T& Max = node->values[node->count - 1];

    return (!(value < Min) && value < Max) || value == Max;
}
/************************************************************************************//*!
 @brief     Checks if a value is in the range of two node's elements.
    
 @param     value
    The value to check.
 @param     left
    The start node to check in.
 @param     right
    The end node to check in.

 @returns   True if the value is in the range.
*//*************************************************************************************/
template <typename T, unsigned int Size>
bool BList<T, Size>::inRange(const T& value, const BNode* left, const BNode* right) const
{
    const T& Min = left->values[0];
    const T& Max = right->values[right->count - 1];

    return (!(value < Min) && value < Max) || value == Max;
}