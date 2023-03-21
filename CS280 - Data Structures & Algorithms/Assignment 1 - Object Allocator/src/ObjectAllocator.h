/************************************************************************************//*!
 \file           ObjectAllocator.hpp
 \author         Diren D Bharwani, diren.dbharwani, 390002520
 \par            email: diren.dbharwani\@digipen.edu
 \date           Jan 19, 2022
 \brief          Contains the interface for the ObjectAllocator class, along with all its
                helper objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef OBJECTALLOCATORH
#define OBJECTALLOCATORH

// Standard Libraries
#include <string>

/*-------------------------------------------------------------------------------------*/
/* Global Variables                                                                    */
/*-------------------------------------------------------------------------------------*/ 

// If the client doesn't specify these:

static const int DEFAULT_OBJECTS_PER_PAGE   = 4;  
static const int DEFAULT_MAX_PAGES          = 3;

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/ 

/************************************************************************************//*!
 @brief  Exception Class.
*//*************************************************************************************/
class OAException
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Possible exception codes
    *//*********************************************************************************/
    enum OA_EXCEPTION 
    {
        E_NO_MEMORY,      //!< out of physical memory (operator new fails)
        E_NO_PAGES,       //!< out of logical memory (max pages has been reached)
        E_BAD_BOUNDARY,   //!< block address is on a page, but not on any block-boundary
        E_MULTIPLE_FREE,  //!< block has already been freed
        E_CORRUPTED_BLOCK //!< block has been corrupted (pad bytes have been overwritten)
    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Default Constructor for OAException

    @param  ErrCode
        One of the 5 error codes listed in OA_EXCEPTION
    @param  Message
        A message returned by the what method.
    *//*********************************************************************************/
    OAException(OA_EXCEPTION ErrCode, const std::string& Message) 
    : error_code_   (ErrCode)
    , message_      (Message) 
    {};

    /********************************************************************************//*!
    @brief  Destructor for OAException
    *//*********************************************************************************/
    virtual ~OAException() {}

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Retrieves the error code

    @return One of the 5 error codes.
    *//*********************************************************************************/
    OA_EXCEPTION code() const { return error_code_; }

    /********************************************************************************//*!
    @brief  Retrieves a human-readable string regarding the error

    @return The NULL-terminated string representing the error.
    *//*********************************************************************************/
    virtual const char *what() const { return message_.c_str(); }

private:  
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    OA_EXCEPTION error_code_; //!< The error code (one of the 5)
    std::string message_;     //!< The formatted string for the user.
};

/************************************************************************************//*!
 @brief  ObjectAllocator configuration parameters
*//*************************************************************************************/
struct OAConfig
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Members                                                             */
    /*---------------------------------------------------------------------------------*/
    static const size_t BASIC_HEADER_SIZE       = sizeof(unsigned) + 1;     //!< allocation number + flags
    static const size_t EXTERNAL_HEADER_SIZE    = sizeof(void*);            //!< just a pointer

    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  The different types of header blocks
    *//*********************************************************************************/
    enum HBLOCK_TYPE
    {
        hbNone, 
        hbBasic, 
        hbExtended, 
        hbExternal
    };

    /********************************************************************************//*!
    @brief  POD that stores the information related to the header blocks.
    *//*********************************************************************************/
    struct HeaderBlockInfo
    {
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        HBLOCK_TYPE type_;          //!< Which of the 4 header types to use?
        size_t      size_;          //!< The size of this header
        size_t      additional_;    //!< How many user-defined additional bytes

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        @brief  Default constructor for HeaderBlockInfo

        @param  type
            The kind of header blocks in use. Defaults to None.
        @param  additional
            The number of user-defined additional bytes required. Defaults to 0.
        *//*****************************************************************************/
        HeaderBlockInfo(HBLOCK_TYPE type = hbNone, unsigned additional = 0) 
        : type_         (type)
        , size_         (0)
        , additional_   (additional)
        {
            if (type_ == hbBasic)
            {
                size_ = BASIC_HEADER_SIZE;
            }
            else if (type_ == hbExtended)
            {
                // alloc # + use counter + flag byte + user-defined
                size_ = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) + additional_;
            } 
            else if (type_ == hbExternal)
            {
                size_ = EXTERNAL_HEADER_SIZE;
            }
        };

    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Default Constructor for OAConfig

    @param  UseCPPMemManager
        Determines whether or not to by-pass the OA. Defaults to false.
    @param  ObjectsPerPage
        Number of objects for each page of memory. Defaults to 4.
    @param  MaxPages
        Maximum number of pages before throwing an exception. 
        A value of 0 means unlimited. Defaults to 3.
    @param  DebugOn
        Sets the debug mode for the allocator. Defaults to false.
    @param  PadBytes
        The number of bytes to the left and right of a block to pad with. Defaults to 0.
    @param  HBInfo
        Information about the header blocks used. 
    @param  Alignment
        The number of bytes to align on. Defaults to 0.
    *//*********************************************************************************/
    OAConfig(bool UseCPPMemManager = false, unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE, unsigned MaxPages = DEFAULT_MAX_PAGES, 
             bool DebugOn = false, unsigned PadBytes = 0, const HeaderBlockInfo &HBInfo = HeaderBlockInfo(), unsigned Alignment = 0) 
    : UseCPPMemManager_ (UseCPPMemManager)
    , ObjectsPerPage_   (ObjectsPerPage)
    , MaxPages_         (MaxPages)
    , DebugOn_          (DebugOn)
    , PadBytes_         (PadBytes)
    , HBlockInfo_       (HBInfo)
    , Alignment_        (Alignment)
    {
        HBlockInfo_     = HBInfo;
        LeftAlignSize_  = 0;  
        InterAlignSize_ = 0;
    }

    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    bool            UseCPPMemManager_;  //!< by-pass the functionality of the OA and use new/delete
    unsigned        ObjectsPerPage_;    //!< number of objects on each page
    unsigned        MaxPages_;          //!< maximum number of pages the OA can allocate (0=unlimited)
    bool            DebugOn_;           //!< enable/disable debugging code (signatures, checks, etc.)
    unsigned        PadBytes_;          //!< size of the left/right padding for each block
    HeaderBlockInfo HBlockInfo_;        //!< size of the header for each block (0=no headers)
    unsigned        Alignment_;         //!< address alignment of each block
    unsigned        LeftAlignSize_;     //!< number of alignment bytes required to align first block
    unsigned        InterAlignSize_;    //!< number of alignment bytes required between remaining blocks
};

/************************************************************************************//*!
 @brief  POD that holds the ObjectAllocator statistical info
*//*************************************************************************************/
struct OAStats
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief  Default constructor for OAStats
    *//*****************************************************************************/
    OAStats() 
    : ObjectSize_   (0)
    , PageSize_     (0)
    , FreeObjects_  (0)
    , ObjectsInUse_ (0)
    , PagesInUse_   (0)
    , MostObjects_  (0)
    , Allocations_  (0)
    , Deallocations_(0) 
    {};

    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    size_t      ObjectSize_;    //!< size of each object
    size_t      PageSize_;      //!< size of a page including all headers, padding, etc.
    unsigned    FreeObjects_;   //!< number of objects on the free list
    unsigned    ObjectsInUse_;  //!< number of objects in use by client
    unsigned    PagesInUse_;    //!< number of pages allocated
    unsigned    MostObjects_;   //!< most objects in use by client at one time
    unsigned    Allocations_;   //!< total requests to allocate memory
    unsigned    Deallocations_; //!< total requests to free memory
};

/************************************************************************************//*!
 @brief  Allows us to easily treat raw objects as nodes in a linked list
*//*************************************************************************************/
struct GenericObject
{
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    GenericObject *Next;    //!< The next object in the list
};

/************************************************************************************//*!
 @brief  Used with external headers
*//*************************************************************************************/
struct MemBlockInfo
{
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    bool in_use;        //!< Is the block free or in use?
    char *label;        //!< A dynamically allocated NUL-terminated string
    unsigned alloc_num; //!< The allocation number (count) of this block
};

/************************************************************************************//*!
 @brief  Encapsulates a custom memory manager
*//*************************************************************************************/
class ObjectAllocator
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    // Defined by the client (pointer to a block, size of block)

    typedef void (*DUMPCALLBACK)(const void *, size_t);     //!< Callback function when dumping memory leaks
    typedef void (*VALIDATECALLBACK)(const void *, size_t); //!< Callback function when validating blocks

    /*---------------------------------------------------------------------------------*/
    /* Static Data Members                                                             */
    /*---------------------------------------------------------------------------------*/
    // Predefined values for memory signatures
    static const unsigned char UNALLOCATED_PATTERN  = 0xAA; //!< New memory never given to the client
    static const unsigned char ALLOCATED_PATTERN    = 0xBB; //!< Memory owned by the client
    static const unsigned char FREED_PATTERN        = 0xCC; //!< Memory returned by the client
    static const unsigned char PAD_PATTERN          = 0xDD; //!< Pad signature to detect buffer over/under flow
    static const unsigned char ALIGN_PATTERN        = 0xEE; //!< For the alignment bytes

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Creates the ObjectAllocator per the specified values

    @param  ObjectSize
        The size of each object in a block.
    @param  config
        The configuration properties for the allocator.

    @throws An exception if the construction fails.
    *//*********************************************************************************/
    ObjectAllocator(size_t ObjectSize, const OAConfig& config);

    // Prevent copy construction and assignment
    ObjectAllocator(const ObjectAllocator &oa) = delete;            //!< Do not implement!
    ObjectAllocator &operator=(const ObjectAllocator &oa) = delete; //!< Do not implement!

    /********************************************************************************//*!
    @brief  Destructor for ObjectAllocator.

    @throws Never does.
    *//*********************************************************************************/
    ~ObjectAllocator();

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Take an object from the free list and give it to the client.

    @param  label
        Dont know whats this for

    @return Pointer to the allocated object.

    @throws An exception from OAException if the object can't be allocated.
    *//*********************************************************************************/
    void* Allocate(const char *label = 0);

    /********************************************************************************//*!
    @brief  Returns an object to the free list for the client.

    @param  Object
        The object to return to the free list.

    @throws An exception from OAException if the object can't be freed.
    *//*********************************************************************************/
    void Free(void *Object);

    /********************************************************************************//*!
    @brief  Calls the callback function for each block still in use.
            Returns the number of blocks in use by the client.

    @param  fn
        The callback function for dumping the memory in use.

    @return Returns the number of blocks in use by the client.
    *//*********************************************************************************/
    unsigned DumpMemoryInUse(DUMPCALLBACK fn) const;

    /********************************************************************************//*!
    @brief  Calls the callback fn for each block that is potentially corrupted.

    @param  fn
        The callback function for dumping the memory in use.

    @return Returns the number of blocks that are corrupted. Only pad bytes are validated.
    *//*********************************************************************************/
    unsigned ValidatePages(VALIDATECALLBACK fn) const;

    /********************************************************************************//*!
    @brief  Returns the number of  pages that was freed
    *//*********************************************************************************/
    unsigned FreeEmptyPages();

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief  Gets the free list.

    @return A pointer to the free list.
    *//*********************************************************************************/
    const void* GetFreeList() const;
    /********************************************************************************//*!
    @brief  Gets the page list.

    @return A pointer to the page list.
    *//*********************************************************************************/
    const void* GetPageList() const;
    /********************************************************************************//*!
    @brief  Gets the configuration of the OA.

    @return The configuration of the OA.
    *//*********************************************************************************/
    OAConfig GetConfig() const;
    /********************************************************************************//*!
    @brief  Gets the statistics of the OA.

    @return The statistics for the OA.
    *//*********************************************************************************/
    OAStats GetStats() const;

    /*---------------------------------------------------------------------------------*/
    /* Setter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Sets the debug mode.

    @param  State
        The state to set the debug mode to.
    *//*********************************************************************************/
    void SetDebugState(bool State);

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/ 
    GenericObject   *PageList_; //!< the beginning of the list of pages
    GenericObject   *FreeList_; //!< the beginning of the list of objects

    OAConfig        config;     //!< the properties of the allocator
    OAStats         stats;      //!< the statistics of the allocator
    size_t          blockSize;  //!< the size of a block in a page

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief  Creates a new page.
    *//*********************************************************************************/
    void createPage();
    /********************************************************************************//*!
    @brief  Inserts a page into the page list.

    @param  page
        The page to insert.
    *//*********************************************************************************/
    void insertPage(GenericObject* page);
    /********************************************************************************//*!
    @brief  Sets up the blocks in an empty page and puts them into the free list.

    @param  page
        The page to set up the blocks in.
    *//*********************************************************************************/
    void setUpBlocks(GenericObject* page);
    /********************************************************************************//*!
    @brief  Frees a single page.

    @param  page
        The page to free.
    *//*********************************************************************************/
    void freePage(GenericObject* page);
    /********************************************************************************//*!
    @brief  Creates a header for a given block. Only call this on Allocate.

    @param  block
        The block to create the header for.
    @param  label 
        The label for an external header. Defaults to a nullptr.
    *//*********************************************************************************/
    void createHeader(unsigned char* block, const char* label = nullptr);
    /********************************************************************************//*!
    @brief  Sets the flag for a header. This is only used for basic and extended headers.

    @param  block
        The block to set the header flag for.
    @param  flag 
        The flag to set. True if in use. False if not.
    *//*********************************************************************************/
    void setHeaderFlag(unsigned char* block, bool flag);
    /********************************************************************************//*!
    @brief  Sets the allocation number for a header. 
            This is only used for basic and extended headers.

    @param  block
        The block to set the header flag for.
    @param  num 
        The allocation number to set.
    *//*********************************************************************************/
    void setHeaderAllocNumber(unsigned char* block, int num);
    /********************************************************************************//*!
    @brief  Destroys a header.

    @param  block
        The block to destroy the header for.
    *//*********************************************************************************/
    void destroyHeader(unsigned char* block);
    /********************************************************************************//*!
    @brief  Increments the stats. Only to be called in Allocate.
    *//*********************************************************************************/
    void incrementStats();
    /********************************************************************************//*!
    @brief  Decrements the stats. Only to be called in Free.
    *//*********************************************************************************/
    void decrementStats();
    /********************************************************************************//*!
    @brief  Computes the alignment for an offset with respect to an alignment value.

    @param  alignment
        The value to align to.
    @param  offset
        The offset between the start and end address.

    @return The alignment amount for the offset with respect to the alignment value.
    *//*********************************************************************************/
    unsigned int computeAlignment(unsigned int alignment, unsigned int offset) const;
    /********************************************************************************//*!
    @brief  Gets the address of the first block in a page.

    @param  page
        The page to get the first block of..

    @return The address of the first block in the page.
    *//*********************************************************************************/
    unsigned char* firstBlock(GenericObject* page) const;
    /********************************************************************************//*!
    @brief  Gets the address of a header for a block.

    @param  block
        The block to get the header of.

    @return The address of the header for the block.
    *//*********************************************************************************/
    unsigned char* header(unsigned char* block) const;
    /********************************************************************************//*!
    @brief  Checks if a page is empty.

    @param  page
        The page to check.

    @return True if the page is empty.
    *//*********************************************************************************/
    bool isPageEmpty(GenericObject* page) const;
    /********************************************************************************//*!
    @brief  Checks if a block is in a page.

    @param  page
        The page to check against
    @param  block
        The block to check for.

    @return True if the block is inside the page.
    *//*********************************************************************************/
    bool isInPage(GenericObject* page, unsigned char* block) const;
    /********************************************************************************//*!
    @brief  Checks if a block as been allocated.

    @param  block
        The block to check for.

    @return True if the block has been allocated.
    *//*********************************************************************************/
    bool isBlockAllocated(unsigned char* block) const;
    /********************************************************************************//*!
    @brief  Sets a pattern for a block. For use in debug mode only.

    @param  block
        The block to set the pattern for.
    @param  pattern
        The pattern to set.
    *//*********************************************************************************/
    void setPattern(unsigned char* block, unsigned char pattern);
    /********************************************************************************//*!
    @brief  Sets the left alignment pattern for a page.

    @param  page
        The page to set the alignment for.
    *//*********************************************************************************/
    void setLeftAlignment(unsigned char* page);
    /********************************************************************************//*!
    @brief  Sets the internal alignment pattern for a block.

    @param  block
        The block to set the alignment for.
    *//*********************************************************************************/
    void setInterAlignment(unsigned char* block);
    /********************************************************************************//*!
    @brief  Checks for an invalid free. For use in debug mode only. Only call in Free.

    @param  data
        The data to check for.

    @throws OAException, based on the type of invalid free.
    *//*********************************************************************************/
    void checkForInvalidFree(void* data) const;
    /********************************************************************************//*!
    @brief  Checks if the data is within any of the allocated pages. 
            For use in debug mode only.
            Always check this first.

    @param  data
        The data to check for.
    @param  currentPage
        An out parameter that indicates the page to check in.

    @throws OAException for bad boundary.
    *//*********************************************************************************/
    void checkWithinPages(unsigned char* data, void*& currentPage) const;
    /********************************************************************************//*!
    @brief  Checks if the data is aligned within a page or to the specified alignment.
            For use in debug mode only.
            MUST BE CALLED AFTER checkWithinPages

    @param  currentPage
        The page to be check in.
    @param  data
        The data to check for.

    @throws OAException for bad boundary.
    *//*********************************************************************************/
    void checkAlignment(void* currentPage, unsigned char* data) const;
    /********************************************************************************//*!
    @brief  Checks if left or right padding has been corrupted.

    @param  block
        The block to check for.

    @return True if corrupted.
    *//*********************************************************************************/
    bool checkCorruption(unsigned char* block) const;
    /********************************************************************************//*!
    @brief  Checks if padding has been corrupted.

    @param  padPtr
        The address of the start of the padding in a sequence.

    @return False if corrupted.
    *//*********************************************************************************/
    bool checkPadding(unsigned char* padPtr) const;
    /********************************************************************************//*!
    @brief  Checks if the block has already been freed.

    @param  block
        The block to check for.
        
    @throws OAException for multiple frees.
    *//*********************************************************************************/
    void checkMultipleFree(unsigned char* block) const;
};

#endif
