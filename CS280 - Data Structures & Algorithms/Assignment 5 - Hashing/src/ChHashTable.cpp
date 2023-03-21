/************************************************************************************//*!
 @file    ChHashTable.cpp
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Apr 10, 2022
 @brief   Contains the interface for the HashTableException, HTStats & ChHashTable
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <exception>
#include <algorithm>
#include <cmath>
// Primary Headers
#include "ChHashTable.h"

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
 @brief     Constructor for ChHashTable

 @param     Config
    The configuration properties for the HornTail.
 @param     allocator
    The memory allocator for the hash table
*//*************************************************************************************/
template <typename T>
ChHashTable<T>::ChHashTable(const HTConfig& Config, ObjectAllocator* allocator)
: heads     { nullptr }
, stats     { nullptr }
, config    { Config }
{
    stats = new HTStats;
    stats->Count_       = 0U;
    stats->TableSize_   = config.InitialTableSize_;
    stats->Probes_      = 0U;
    stats->Expansions_  = 0U;
    stats->HashFunc_    = config.HashFunc_;
    stats->Allocator_   = allocator;

    try
    {
        heads = new ChHTHeadNode[config.InitialTableSize_];
    }
    catch (const std::bad_alloc&)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        throw HashTableException
        (
            HashTableException::HASHTABLE_EXCEPTION::E_NO_MEMORY,
            functionSignature + ": No memory left to allocate!"
        );
    }
}
/************************************************************************************//*!
 @brief     Destructor for ChHashTable
*//*************************************************************************************/
template <typename T>
ChHashTable<T>::~ChHashTable()
{
    for (unsigned int i = 0; i < stats->TableSize_; ++i)
    {
        // Delete nodes in each list
        deleteHead(heads + i);
    }

    delete[] heads;
    delete stats;
}

/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Getter for the stats of a ChHashTable.
 
 @returns   The stats for a ChHashTable.
*//*************************************************************************************/
template <typename T>
HTStats ChHashTable<T>::GetStats() const
{
    return *stats;
}
/************************************************************************************//*!
 @brief     Getter for the table of a ChHashTable.
 
 @returns   A pointer to the start of the array of head nodes.
*//*************************************************************************************/
template <typename T>
const typename ChHashTable<T>::ChHTHeadNode* ChHashTable<T>::GetTable() const
{
    return heads;
}

/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Inserts data into the Hash Table
 
 @param     Key
    The key for the value
 @param     Data
    The data to insert
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::insert(const char* Key, const T& Data)
{
    const double LOAD_FACTOR = static_cast<double>(stats->Count_ + 1U) / stats->TableSize_;
    if (LOAD_FACTOR > config.MaxLoadFactor_)
    {
        growTable();
    }

    const uint32_t INDEX = stats->HashFunc_(Key, stats->TableSize_);
    insertNode(heads + INDEX, Key, Data);
}
/************************************************************************************//*!
 @brief     Deletes an item by key
 
 @param     Key
    The key for the value to delete
    
 @throws    E_ITEM_NOT_FOUND
    If the key has not been found
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::remove(const char* Key)
{
    // Hash key to get the index of the element
    const uint32_t INDEX = stats->HashFunc_(Key, stats->TableSize_);

    ChHTNode* prev = nullptr;
    ChHTNode* temp = heads[INDEX].Nodes;
    ++stats->Probes_;

    // Find node with matching key
    bool keyFound = false;
    while (temp)
    {
        if (!strcmp(temp->Key, Key))
        {
            keyFound = true;
            break;
        }

        prev = temp;
        temp = temp->Next;

        // Track probes (searches)
        ++stats->Probes_;
    }

    if (!keyFound)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        throw HashTableException
        (
            HashTableException::HASHTABLE_EXCEPTION::E_ITEM_NOT_FOUND,
            functionSignature + ": Attempted to insert duplicate data!"
        );
    }

    // Reassign
    if (!prev)
    {
        // replace the head
        ChHTNode* newHead = temp->Next;
        heads[INDEX].Nodes = newHead;
    }
    else
    {
        ChHTNode* next = temp->Next;
        prev->Next = next;
    }

    deleteNode(temp);

    --stats->Count_;
    --heads[INDEX].Count;
}
/************************************************************************************//*!
 @brief     Find data by a given key.

 @param     Key
    The key for the value to delete

 @returns   The data found by the key

 @throws    E_ITEM_NOT_FOUND
    If the key has not been found
*//*************************************************************************************/
template <typename T>
const T& ChHashTable<T>::find(const char* Key) const
{
    const uint32_t INDEX = stats->HashFunc_(Key, stats->TableSize_);

    ChHTNode* temp = heads[INDEX].Nodes;
    ++stats->Probes_;

    while (temp)
    {
        if (!strcmp(temp->Key, Key))
        {
            return temp->Data;
        }
        temp = temp->Next;

        ++stats->Probes_;
    }

    std::string functionSignature;
    #ifdef _MSC_VER
    functionSignature = __FUNCTION__;
    #else
    functionSignature = __PRETTY_FUNCTION__;
    #endif

    throw HashTableException
    (
        HashTableException::HASHTABLE_EXCEPTION::E_ITEM_NOT_FOUND,
        functionSignature + ": Attempted to insert duplicate data!"
    );
}
/************************************************************************************//*!
 @brief     Removes all items from the table (Doesn't deallocate table)
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::clear()
{
    for (unsigned int i = 0; i < stats->TableSize_; ++i)
    {
        // Delete nodes in each list
        deleteHead(heads + i);
    }

    stats->Count_ = 0U;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
/************************************************************************************//*!
 @brief     Inserts a node at the head of a list
 
 @param     head
    The head of the list to insert into.
 @param     key
    The key of the new node.
 @param     data
    The data of the new node
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::insertNode(ChHTHeadNode* head, const char* key, const T& data)
{
    ChHTNode* temp = head->Nodes;
    ++stats->Probes_;

    // Check for duplicate
    while (temp)
    {
        if (!strcmp(temp->Key, key))
        {
            std::string functionSignature;
            #ifdef _MSC_VER
            functionSignature = __FUNCTION__;
            #else
            functionSignature = __PRETTY_FUNCTION__;
            #endif

            throw HashTableException
            (
                HashTableException::HASHTABLE_EXCEPTION::E_DUPLICATE,
                functionSignature + ": Attempted to insert duplicate data!"
            );
        }
        temp = temp->Next;

        // Track probes (searches)
        ++stats->Probes_;
    }

    try
    {
        ChHTNode* newNode;
        if (stats->Allocator_)
        {
            newNode = static_cast<ChHTNode*>(stats->Allocator_->Allocate());
            newNode->Data = data;
        }
        else
        {
            newNode = new ChHTNode(data);
        }

        #ifdef _MSC_VER
        strcpy_s(newNode->Key, key);
        #else
        strcpy(newNode->Key, key);
        #endif
        newNode->Next   = head->Nodes;
        head->Nodes     = newNode;

        ++stats->Count_;
        ++head->Count;
    }
    catch (const std::bad_alloc&)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        throw HashTableException
        (
            HashTableException::HASHTABLE_EXCEPTION::E_NO_MEMORY,
            functionSignature + ": No memory left to allocate a new node!"
        );
    }
}
/************************************************************************************//*!
 @brief     Deletes a node from the hash table
 
 @param     node
    The node to delete.
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::deleteNode(ChHTNode* node)
{
    // Remove
    if (config.FreeProc_)
    {
        config.FreeProc_(node->Data);
    }

    if (node)
    {
        if (stats->Allocator_)
        {
            stats->Allocator_->Free(node);
        }
        else
        {
            delete node;
        }

        node = nullptr;
    }
}
/************************************************************************************//*!
 @brief     Deletes all the nodes in a head node.
 
 @param     head
    The head to delete.
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::deleteHead(ChHTHeadNode* head)
{
    ChHTNode* temp = head->Nodes;
    while (temp)
    {
        ChHTNode* next = temp->Next;
        deleteNode(temp);
        temp = next;
    }
    temp = nullptr;

    head->Count = 0;
    head->Nodes = nullptr;
}
/************************************************************************************//*!
 @brief     Grows the table
*//*************************************************************************************/
template <typename T>
void ChHashTable<T>::growTable()
{
    const double        ACTUAL_GROWTH_FACTOR    = std::ceil(stats->TableSize_ * config.GrowthFactor_);
    const unsigned int  NEW_SIZE                = GetClosestPrime(static_cast<unsigned int>(ACTUAL_GROWTH_FACTOR));

    // Create a new set of lists
    ChHTHeadNode* newHead = nullptr;

    try
    {
        newHead = new ChHTHeadNode[NEW_SIZE];
    }
    catch (const std::bad_alloc&)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        throw HashTableException
        (
            HashTableException::HASHTABLE_EXCEPTION::E_NO_MEMORY,
            functionSignature + ": No memory left to allocate!"
        );
    }

    const unsigned int OLD_SIZE = stats->TableSize_;
    stats->TableSize_ = static_cast<unsigned int>(NEW_SIZE);

    // Reset count
    stats->Count_ = 0U;

    // Traverse current heads and reinsert
    for (unsigned int i = 0; i < OLD_SIZE; ++i)
    {
        ChHTNode* temp = heads[i].Nodes;

        while (temp)
        {
            const uint32_t NEW_INDEX = stats->HashFunc_(temp->Key, stats->TableSize_);
            insertNode(newHead + NEW_INDEX, temp->Key, temp->Data);

            // Next node
            ChHTNode* next = temp->Next;
            deleteNode(temp);
            temp = next;
        }
    }

    // Delete Old Heads
    delete[] heads;
    heads = newHead;

    ++stats->Expansions_;
}




