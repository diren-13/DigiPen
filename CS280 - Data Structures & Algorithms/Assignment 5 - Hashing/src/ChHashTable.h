/************************************************************************************//*!
 @file    ChHashTable.h
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Apr 10, 2022
 @brief   Contains the interface for the HashTableException, HTStats & ChHashTable
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef CHHASHTABLEH
#define CHHASHTABLEH

// Standard Libraries
#include <string>
// Project Headers
#include "ObjectAllocator.h"
#include "support.h"

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
 @brief     The maximum length of a key
*//*************************************************************************************/
const unsigned int MAX_KEYLEN = 10;

/************************************************************************************//*!
 @brief     Client-provided hash function

 @param     key
    The key for the hash value
 @param     tableSize
    The size of the table

 @return    Returns an index in the table
*//*************************************************************************************/
typedef unsigned int (*HASHFUNC)(const char* key, unsigned int tableSize);

/************************************************************************************//*!
 @brief     Encapsulates exception types for a Hash Table.
*//*************************************************************************************/
class HashTableException
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
     @brief     The type of the exception.
    *//*********************************************************************************/
    enum HASHTABLE_EXCEPTION
    {
        E_ITEM_NOT_FOUND,
        E_DUPLICATE,
        E_NO_MEMORY
    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
     @brief     Constructor for a HashTableException

     @param     ErrCode
        The error code
     @param     Message
        A string that describes the exception.
    *//*********************************************************************************/
    HashTableException(int ErrCode, const std::string& Message)
    : error_code_   (ErrCode)
    , message_      (Message)
    {};

    /********************************************************************************//*!
     @brief     Destructor for a HashTableException
    *//*********************************************************************************/
    virtual ~HashTableException()
    {}

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    virtual int         code()  const   { return error_code_; }
    virtual const char* what()  const   { return message_.c_str(); }

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    int         error_code_;
    std::string message_;

};


/************************************************************************************//*!
 @brief     HashTable statistical info
*//*************************************************************************************/
struct HTStats
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    unsigned            Count_;         // Number of elements in the table
    unsigned            TableSize_;     // Size of the table (total slots)
    unsigned            Probes_;        // Number of probes performed
    unsigned            Expansions_;    // Number of times the table grew
    HASHFUNC            HashFunc_;      // Pointer to primary hash function
    ObjectAllocator*    Allocator_;     // The allocator in use (may be 0)

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    HTStats()
    : Count_        (0)
    , TableSize_    (0)
    , Probes_       (0)
    , Expansions_   (0)
    , HashFunc_     (0)
    , Allocator_    (nullptr)
    {};
};

/************************************************************************************//*!
 @brief     Encapsulates a Chaos HornTail.
*//*************************************************************************************/
template <typename T>
class ChHashTable
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
     @brief     Callback for freeing nodes
    *//*********************************************************************************/
    typedef void (*FREEPROC)(T); // client-provided free proc (we own the data)

    /********************************************************************************//*!
     @brief     Encapsulates the properties of the HornTail.
    *//*********************************************************************************/
    struct HTConfig
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        unsigned    InitialTableSize_;  // The number of slots in the table initially.
        double      MaxLoadFactor_;     // The maximum "fullness" of the table.
        double      GrowthFactor_;      // The factor by which the table grows.
        HASHFUNC    HashFunc_;          // The hash function used in all cases.
        FREEPROC    FreeProc_;          // The method provided by the client that may need to be called when data in the table is removed.
                                        

        /*-----------------------------------------------------------------------------*/
        /* Constructor                                                                 */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
         @brief     Constructor for HTConfig
        *//*****************************************************************************/
        HTConfig
        (
            unsigned    InitialTableSize,
            HASHFUNC    HashFunc, 
            double      MaxLoadFactor       = 3.0, 
            double      GrowthFactor        = 2.0,
            FREEPROC    FreeProc            = 0
        )
        : InitialTableSize_ (InitialTableSize)
        , MaxLoadFactor_    (MaxLoadFactor)
        , GrowthFactor_     (GrowthFactor)
        , HashFunc_         (HashFunc)
        , FreeProc_         (FreeProc)
        {}
    };

    /********************************************************************************//*!
     @brief     Encapsulates a Chaos HornTail node.
    *//*********************************************************************************/
    struct ChHTNode
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        char        Key[MAX_KEYLEN];    // Key is a string
        T           Data;               // Client data
        ChHTNode*   Next;               // Pointer to the next horntail

        /*-----------------------------------------------------------------------------*/
        /* Constructor                                                                 */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
         @brief     Constructor for a ChHTNode

         @param     data
            The data for the node.
        *//*****************************************************************************/
        ChHTNode(const T& data)
        : Data  (data)
        , Next  (nullptr)
        {};
    };

    /********************************************************************************//*!
     @brief     Encapsulates a Head on the Chaos HornTail.
    *//*********************************************************************************/
    struct ChHTHeadNode
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        ChHTNode*   Nodes;
        int         Count; 

        /*-----------------------------------------------------------------------------*/
        /* Constructor                                                                 */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
         @brief     Constructor for a ChHTHeadNode
        *//*****************************************************************************/
        ChHTHeadNode()
        : Nodes(0)
        , Count(0)
        {};
    };

    /********************************************************************************//*!
     @brief     Constructor for ChHashTable

     @param     Config
        The configuration properties for the HornTail.
     @param     allocator
        The memory allocator for the hash table
    *//*********************************************************************************/
    ChHashTable(const HTConfig& Config, ObjectAllocator* allocator = 0);
    /********************************************************************************//*!
     @brief     Destructor for ChHashTable
    *//*********************************************************************************/
    ~ChHashTable();

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
     @brief     Getter for the stats of a ChHashTable.

     @returns   The stats for a ChHashTable
    *//*********************************************************************************/
    HTStats             GetStats() const;
    /********************************************************************************//*!
     @brief     Getter for the table of a ChHashTable.

     @returns   A pointer to the start of the array of head nodes.
    *//*********************************************************************************/
    const ChHTHeadNode* GetTable() const;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
     @brief     Inserts data into the Hash Table

     @param     Key
        The key for the value
     @param     Data
        The data to insert
    *//*********************************************************************************/
    void insert(const char* Key, const T& Data);

    /********************************************************************************//*!
     @brief     Deletes an item by key

     @param     Key
        The key for the value to delete

     @throws    E_ITEM_NOT_FOUND
        If the key has not been found
    *//*********************************************************************************/
    void remove(const char* Key);

    /********************************************************************************//*!
     @brief     Find data by a given key.

     @param     Key
        The key for the value to delete

     @returns   The data found by the key

     @throws    E_ITEM_NOT_FOUND
        If the key has not been found
    *//*********************************************************************************/
    const T& find(const char* Key) const;

    /********************************************************************************//*!
     @brief     Removes all items from the table (Doesn't deallocate table)
    *//*********************************************************************************/
    void clear();

  private:

    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    ChHTHeadNode*       heads;
    HTStats*            stats;

    HTConfig            config;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
     @brief     Inserts a node at the head of a list

     @param     head
        The head of the list to insert into.
     @param     key
        The key of the new node.
     @param     data
        The data of the new node
    *//*********************************************************************************/
    void insertNode     (ChHTHeadNode* head, const char* key, const T& data);
    /********************************************************************************//*!
     @brief     Deletes a node from the hash table

     @param     node
        The node to delete.
    *//*********************************************************************************/
    void deleteNode     (ChHTNode* node);
    /********************************************************************************//*!
     @brief     Deletes all the nodes in a head node.

     @param     head
        The head to delete.
    *//*********************************************************************************/
    void deleteHead     (ChHTHeadNode* head);
    /********************************************************************************//*!
     @brief     Grows the table
    *//*********************************************************************************/
    void growTable      ();

};

#include "ChHashTable.cpp"

#endif
