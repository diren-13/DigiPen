/************************************************************************************//*!
\file           ObjectAllocator.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Jan 19, 2022
\brief          Contains the implementation of the Matrix class.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "ObjectAllocator.h"
// Standard Libraries
#include <iostream>
#include <cstring>

#define PTR_SIZE sizeof(unsigned char*)

#define TO_GENERIC_OBJECT_PTR(pointer) reinterpret_cast<GenericObject*>(pointer)
#define TO_UCHAR_PTR(pointer) reinterpret_cast<unsigned char*>(pointer)

#define MAX(x, y) x > y ? x : y

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/

/********************************************************************************//*!
 @brief  Creates the ObjectAllocator per the specified values

 @param  ObjectSize
        The size of each object in a block.
 @param  config
        The configuration properties for the allocator.

 @throws An exception if the construction fails.
*//*********************************************************************************/
ObjectAllocator::ObjectAllocator(size_t objectSize, const OAConfig& c)
: PageList_     (nullptr)
, FreeList_     (nullptr)
, config        (c)
, stats         ()
, blockSize     (0)
{
    // Populate stats
    stats.ObjectSize_ = objectSize;

    unsigned int leftOffset = static_cast<unsigned int>(c.HBlockInfo_.size_ + c.PadBytes_ + PTR_SIZE);
    config.LeftAlignSize_ = computeAlignment(c.Alignment_, leftOffset);

    unsigned int interOffset = static_cast<unsigned int>(objectSize + c.HBlockInfo_.size_ + 2 * c.PadBytes_);
    config.InterAlignSize_ = computeAlignment(c.Alignment_, interOffset);

    blockSize = objectSize;
    blockSize += (2 * c.PadBytes_);             // Add padding
    blockSize += c.HBlockInfo_.size_ ;          // Add header
    blockSize += config.InterAlignSize_;        // Add alignment
    
    stats.PageSize_ = PTR_SIZE;
    stats.PageSize_ += config.LeftAlignSize_;
    stats.PageSize_ += blockSize * c.ObjectsPerPage_;
    // Remove one interAlignment
    stats.PageSize_ -= config.InterAlignSize_;

    if (config.UseCPPMemManager_)
        return;

    createPage();
}

/********************************************************************************//*!
 @brief  Destructor for ObjectAllocator.

 @throws Never does.
*//*********************************************************************************/
ObjectAllocator::~ObjectAllocator()
{
    while(PageList_ != nullptr)
    {
        GenericObject* next = PageList_->Next;
        delete[] TO_UCHAR_PTR(PageList_);
        PageList_ = next;
    }
}

/*-------------------------------------------------------------------------------------*/
/* Function Members                                                                    */
/*-------------------------------------------------------------------------------------*/

/********************************************************************************//*!
 @brief  Take an object from the free list and give it to the client.

 @param  label
    Dont know whats this for

 @return Pointer to the allocated object.

 @throws An exception from OAException if the object can't be allocated.
*//*********************************************************************************/
void* ObjectAllocator::Allocate(const char* label)
{
    if (config.UseCPPMemManager_)
    {
        try
        {
            unsigned char* obj = new unsigned char[stats.ObjectSize_];
            ++stats.Allocations_;
            ++stats.ObjectsInUse_;
            stats.MostObjects_ = MAX(stats.MostObjects_, stats.ObjectsInUse_);
            return obj;
        }
        catch(const std::bad_alloc&)
        {
            throw OAException{OAException::E_NO_MEMORY, "No physical memory left."};
        }
    }

    // Take block from free list
    if (FreeList_ == nullptr)
    {
        createPage();
    }

    unsigned char* obj = TO_UCHAR_PTR(FreeList_);
    FreeList_ = FreeList_->Next;

    setPattern(obj, ALLOCATED_PATTERN);
    incrementStats();
    createHeader(obj, label);

    return obj;
}

/********************************************************************************//*!
 @brief  Returns an object to the free list for the client.

 @param  Object
    The object to return to the free list.

 @throws An exception from OAException if the object can't be freed.
*//*********************************************************************************/
void ObjectAllocator::Free(void* Object)
{
    if (config.UseCPPMemManager_)
    {
        delete[] TO_UCHAR_PTR(Object);

        ++stats.Deallocations_;
        --stats.ObjectsInUse_;

        return;
    }

    checkForInvalidFree(Object);

    GenericObject* temp = TO_GENERIC_OBJECT_PTR(Object);
    temp->Next = FreeList_;
    FreeList_ = temp;

    unsigned char* cFL = TO_UCHAR_PTR(FreeList_);
    setPattern(cFL, FREED_PATTERN);
    destroyHeader(cFL);

    decrementStats();
}

/********************************************************************************//*!
 @brief  Calls the callback function for each block still in use.
        Returns the number of blocks in use by the client.

 @param  fn
    The callback function for dumping the memory in use.

 @return Returns the number of blocks in use by the client.
*//*********************************************************************************/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
    if (config.HBlockInfo_.type_ == OAConfig::hbNone)
        return stats.ObjectsInUse_;

    // Traverse pages
    GenericObject* page = PageList_;
    for (size_t i = 0; i < stats.PagesInUse_; ++i)
    {
        unsigned char* block = firstBlock(page);
        for (size_t j = 0; j < config.ObjectsPerPage_; ++j)
        {
            switch (config.HBlockInfo_.type_)
            {
                case OAConfig::hbBasic:
                case OAConfig::hbExtended:
                {
                    // Check flag for each header
                    unsigned char* flag = block - config.PadBytes_ - sizeof(char);
                    if (*flag)
                    {
                        fn(block, stats.ObjectSize_);
                    }
                    break;
                }
                case OAConfig::hbExternal:
                {
                    unsigned char* header = block - config.PadBytes_ - config.HBlockInfo_.size_;
                    MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);
                    if (*info && (*info)->in_use)
                    {
                        fn(block, stats.ObjectSize_);
                    }
                    break;
                }
                default: break;
            }
            block += blockSize;
        }
        page = page->Next;
    }

    return stats.ObjectsInUse_;
}

/********************************************************************************//*!
 @brief  Calls the callback fn for each block that is potentially corrupted.

 @param  fn
    The callback function for dumping the memory in use.

 @return Returns the number of blocks that are corrupted. Only pad bytes are validated.
*//*********************************************************************************/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
    unsigned int numCorrupted = 0;

    // Traverse pages
    GenericObject* page = PageList_;
    for (size_t i = 0; i < stats.PagesInUse_; ++i)
    {
        unsigned char* block = firstBlock(page);
        for (size_t j = 0; j < config.ObjectsPerPage_; ++j)
        {
            // Check corruption in each block
            if (checkCorruption(block))
            {
                ++numCorrupted;
                fn(block, stats.ObjectSize_);
            }
            block += blockSize;
        }
        page = page->Next;
    }

    return numCorrupted;
}

/********************************************************************************//*!
 @brief  Returns the number of  pages that was freed
*//*********************************************************************************/
unsigned ObjectAllocator::FreeEmptyPages()
{
    if (PageList_ == nullptr)
        return 0; 

    unsigned int numFreed = 0;
    
    // Traverse pages
    GenericObject* page = PageList_;
    GenericObject* prev = nullptr;

    while (page != nullptr)
    {
        if (isPageEmpty(page))
        {
            GenericObject* next = page->Next;
            freePage(page);
            (prev ? prev->Next : PageList_) = next;
            page = next;

            ++numFreed;
        }
        else
        {
            prev = page;
            page = page->Next;
        }
    }

    return numFreed;
}

/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/ 
/********************************************************************************//*!
 @brief  Gets the free list.

 @return A pointer to the free list.
*//*********************************************************************************/
const void* ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}
/********************************************************************************//*!
 @brief  Gets the page list.

 @return A pointer to the page list.
*//*********************************************************************************/
const void* ObjectAllocator::GetPageList() const
{
    return PageList_;
}
/********************************************************************************//*!
 @brief  Gets the configuration of the OA.

 @return The configuration of the OA.
*//*********************************************************************************/
OAConfig ObjectAllocator::GetConfig() const
{
    return config;
}
/********************************************************************************//*!
 @brief  Gets the statistics of the OA.

 @return The statistics for the OA.
*//*********************************************************************************/
OAStats ObjectAllocator::GetStats() const
{
    return stats;
}

/*-------------------------------------------------------------------------------------*/
/* Setter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/
/********************************************************************************//*!
 @brief  Sets the debug mode.

 @param  State
    The state to set the debug mode to.
*//*********************************************************************************/
void ObjectAllocator::SetDebugState(bool state)
{
    config.DebugOn_ = state;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
/********************************************************************************//*!
 @brief  Creates a new page.
*//*********************************************************************************/
void ObjectAllocator::createPage()
{
    if (config.MaxPages_ && stats.PagesInUse_ == config.MaxPages_)
    {
        throw OAException {OAException::E_NO_PAGES, "Maximum number of pages have been reached."};
    }

    try
    {
        unsigned char* page = new unsigned char[stats.PageSize_];
        GenericObject* p = TO_GENERIC_OBJECT_PTR(page);
            
        insertPage(p);
        setUpBlocks(p);

        // Update stats
        ++stats.PagesInUse_;
        stats.FreeObjects_ += config.ObjectsPerPage_;
    }
    catch(const std::bad_alloc&)
    {
        throw OAException {OAException::E_NO_MEMORY, "No system memory available."};
    }
}
/********************************************************************************//*!
 @brief  Inserts a page into the page list.

 @param  page
    The page to insert.
*//*********************************************************************************/
void ObjectAllocator::insertPage(GenericObject* page)
{
    // PageList points to new page, new page next points to previous page
    GenericObject* currentPage = PageList_;
    PageList_ = page;
    PageList_->Next = stats.PagesInUse_ ? currentPage : nullptr;
}
/********************************************************************************//*!
 @brief  Sets up the blocks in an empty page and puts them into the free list.

 @param  page
    The page to set up the blocks in.
*//*********************************************************************************/
void ObjectAllocator::setUpBlocks(GenericObject* page)
{
    unsigned char* cP = TO_UCHAR_PTR(page);
    unsigned char* cFL = TO_UCHAR_PTR(FreeList_);

    setLeftAlignment(cP);

    // Prepare blocks and assign FreeList to point to the last block.
    for (size_t i = 0; i < config.ObjectsPerPage_; ++i)
    {
        size_t offset = i ? blockSize : PTR_SIZE + config.PadBytes_ + config.HBlockInfo_.size_ + config.LeftAlignSize_;

        GenericObject* currentBlock = FreeList_;
        GenericObject* nextBlock    = TO_GENERIC_OBJECT_PTR(cFL + offset);

        FreeList_ = i ? nextBlock : TO_GENERIC_OBJECT_PTR(cP + offset);
        FreeList_->Next = i ? currentBlock : nullptr;

        cFL = TO_UCHAR_PTR(FreeList_);

        // Set headers 
        switch (config.HBlockInfo_.type_)
        {
            case OAConfig::hbBasic:
            case OAConfig::hbExtended:
            {
                setHeaderFlag(cFL, false);
                setHeaderAllocNumber(cFL, 0);

                // Set use count and user-defined bytes for extended header
                if (config.HBlockInfo_.type_ == OAConfig::hbExtended)
                {
                    size_t numBytes = config.HBlockInfo_.size_ - sizeof(unsigned char) - sizeof(int);
                    memset(header(cFL), 0, numBytes);
                }

                break;
            }
            case OAConfig::hbExternal:
            {
                memset(header(cFL), 0, config.HBlockInfo_.size_);
                break;
            }
            default: break;
        } 

        if (i != config.ObjectsPerPage_ -1)
        {
            setInterAlignment(cFL);
        }
        setPattern(cFL, PAD_PATTERN);
        setPattern(cFL, UNALLOCATED_PATTERN);
    }
}

/********************************************************************************//*!
 @brief  Frees a single page.

 @param  page
    The page to free.
*//*********************************************************************************/
void ObjectAllocator::freePage(GenericObject* page)
{
    GenericObject* fL = FreeList_;
    GenericObject* prev = nullptr;

    // Traverse freelist
    while (fL != nullptr)
    {
        unsigned char* cFL = TO_UCHAR_PTR(fL);
        if (isInPage(page, cFL))
        {
            GenericObject* next = fL->Next;
            (prev ? prev->Next : FreeList_) = next;
            --stats.FreeObjects_;
        }
        else
        {
            prev = fL;
        }
        fL = fL->Next;
    }

    delete[] TO_UCHAR_PTR(page);
    --stats.PagesInUse_;
}
/********************************************************************************//*!
 @brief  Creates a header for a given block. Only call this on Allocate.

 @param  block
    The block to create the header for.
 @param  label 
    The label for an external header. Defaults to a nullptr.
*//*********************************************************************************/
void ObjectAllocator::createHeader(unsigned char* block, const char* label)
{
    switch (config.HBlockInfo_.type_)
    {
        case OAConfig::hbBasic:
        case OAConfig::hbExtended:
        {
            setHeaderFlag(block, true);
            setHeaderAllocNumber(block, stats.Allocations_);
            
            // Set use count for extended headers
            if (config.HBlockInfo_.type_ == OAConfig::hbExtended)
            {
                size_t userBytes = config.HBlockInfo_.size_ - (OAConfig::BASIC_HEADER_SIZE + sizeof(short));
                short* useCount = reinterpret_cast<short*>(header(block) + userBytes);
                ++(*useCount);
            }

            break;
        }
        case OAConfig::hbExternal:
        {
            // Move to header pointer
            unsigned char* header = block - static_cast<size_t>(config.PadBytes_) - config.HBlockInfo_.size_;
            MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

            try
            {
                *info = new MemBlockInfo;
                (*info)->alloc_num  = stats.Allocations_;
                (*info)->in_use     = true;
                
                if (label)
                {
                    (*info)->label = new char[strlen(label) + 1];
                    strcpy_s((*info)->label, strlen(label) + 1, label);
                }
                else
                {
                    (*info)->label = nullptr;
                }
            }
            catch(const std::bad_alloc&)
            {
                throw OAException{OAException::E_NO_MEMORY, "No physical memory available!"};
            }

            break;
        }
        default: break;
    }
}
/********************************************************************************//*!
 @brief  Sets the flag for a header. This is only used for basic and extended headers.

 @param  block
    The block to set the header flag for.
 @param  flag 
    The flag to set. True if in use. False if not.
*//*********************************************************************************/
void ObjectAllocator::setHeaderFlag(unsigned char* block, bool flag)
{
    size_t offset = static_cast<size_t>(config.PadBytes_) + sizeof(char);
    unsigned char* pFlag = block - offset;
    *pFlag = static_cast<unsigned char>(flag);
}
/********************************************************************************//*!
 @brief  Sets the allocation number for a header. 
        This is only used for basic and extended headers.

 @param  block
    The block to set the header flag for.
 @param  num 
    The allocation number to set.
*//*********************************************************************************/
void ObjectAllocator::setHeaderAllocNumber(unsigned char* block, int num)
{
    size_t offset = static_cast<size_t>(config.PadBytes_) + sizeof(char) + sizeof(int);
    int* allocNum = reinterpret_cast<int*>(block - offset);
    *allocNum = num;
}
/********************************************************************************//*!
 @brief  Destroys a header.

 @param  block
    The block to destroy the header for.
*//*********************************************************************************/
void ObjectAllocator::destroyHeader(unsigned char* block)
{
    switch (config.HBlockInfo_.type_)
    {
        case OAConfig::hbBasic:
        case OAConfig::hbExtended:
        {
            setHeaderFlag(block, false);
            setHeaderAllocNumber(block, 0);

            break;
        }
        case OAConfig::hbExternal:
        {
            // Move to header pointer
            unsigned char* header = block - static_cast<size_t>(config.PadBytes_) - config.HBlockInfo_.size_;
            MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

            // delete label & info
            if (*info)
            {
                delete[] (*info)->label;
                delete *info;
            }

            memset(header, 0, config.HBlockInfo_.size_);
            break;
        }
        default: break;
    }
}
/********************************************************************************//*!
 @brief  Increments the stats. Only to be called in Allocate.
*//*********************************************************************************/
void ObjectAllocator::incrementStats()
{
    ++stats.Allocations_;
    ++stats.ObjectsInUse_;
    --stats.FreeObjects_;
    stats.MostObjects_ = MAX(stats.MostObjects_, stats.ObjectsInUse_);
}
/********************************************************************************//*!
 @brief  Decrements the stats. Only to be called in Free.
*//*********************************************************************************/
void ObjectAllocator::decrementStats()
{
    ++stats.Deallocations_;
    ++stats.FreeObjects_;
    --stats.ObjectsInUse_;
}
/********************************************************************************//*!
 @brief  Computes the alignment for an offset with respect to an alignment value.

 @param  alignment
    The value to align to.
 @param  offset
    The offset between the start and end address.

 @return The alignment amount for the offset with respect to the alignment value.
*//*********************************************************************************/
unsigned int ObjectAllocator::computeAlignment(unsigned int alignment, unsigned int offset) const
{
    if (config.Alignment_ <= 1)
        return 0;

    const unsigned int closestMultiple = offset / alignment + 1;
    return (alignment * closestMultiple) - offset;
}
/********************************************************************************//*!
 @brief  Gets the address of the first block in a page.

 @param  page
    The page to get the first block of..

 @return The address of the first block in the page.
*//*********************************************************************************/
unsigned char* ObjectAllocator::firstBlock(GenericObject* page) const
{
    return TO_UCHAR_PTR(page) + PTR_SIZE + config.LeftAlignSize_ + config.HBlockInfo_.size_ + config.PadBytes_;
}
/********************************************************************************//*!
 @brief  Gets the address of a header for a block.

 @param  block
    The block to get the header of.

 @return The address of the header for the block.
*//*********************************************************************************/
unsigned char* ObjectAllocator::header(unsigned char* block) const
{
    return block - (config.PadBytes_ + config.HBlockInfo_.size_); 
}
/********************************************************************************//*!
 @brief  Checks if a page is empty.

 @param  page
    The page to check.

 @return True if the page is empty.
*//*********************************************************************************/
bool ObjectAllocator::isPageEmpty(GenericObject* page) const
{
    GenericObject* fL = this->FreeList_;
    unsigned int freeBlocks = 0;

    while (fL)
    {
        unsigned char* cFL = TO_UCHAR_PTR(fL);
        if (isInPage(page, cFL))
        {
            if (++freeBlocks >= config.ObjectsPerPage_)
                return true;
        }
        fL = fL->Next;
    }
    return false;
}
/********************************************************************************//*!
 @brief  Checks if a block is in a page.

 @param  page
    The page to check against
 @param  block
    The block to check for.

 @return True if the block is inside the page.
*//*********************************************************************************/
bool ObjectAllocator::isInPage(GenericObject* page, unsigned char* block) const
{
    unsigned char* cP = TO_UCHAR_PTR(page);
    return block > cP && block < (cP + stats.PageSize_);
}
/********************************************************************************//*!
 @brief  Checks if a block as been allocated.

 @param  block
    The block to check for.

 @return True if the block has been allocated.
*//*********************************************************************************/
bool ObjectAllocator::isBlockAllocated(unsigned char* block) const
{
    switch (config.HBlockInfo_.type_)
    {
        case OAConfig::hbNone:
        {
            GenericObject* b    = TO_GENERIC_OBJECT_PTR(block);
            GenericObject* fL   = FreeList_;

            // Iterate through free list and compare addresses
            while (fL != nullptr)
            {
                // If block is on the free list, it is unallocated
                if (fL == b)
                    return false;
                
                fL = fL->Next;
            }

            // Block cannot be found on the free list, therefore it has been allocated
            return true;
        }   
        default: break;
    }

    return false;
}
/********************************************************************************//*!
 @brief  Sets a pattern for a block. For use in debug mode only.

 @param  block
    The block to set the pattern for.
 @param  pattern
    The pattern to set.
*//*********************************************************************************/
void ObjectAllocator::setPattern(unsigned char* block, unsigned char pattern)
{
    if(!config.DebugOn_)
        return;

    size_t interval = 0;

    switch (pattern)
    {
        case UNALLOCATED_PATTERN:
        case FREED_PATTERN:
        {
            unsigned char* patternHead = block + PTR_SIZE;
            interval = stats.ObjectSize_ - PTR_SIZE;

            memset(patternHead, pattern, interval);

            break;
        }
        case ALLOCATED_PATTERN:
        {
            unsigned char* patternHead = block;
            interval = stats.ObjectSize_;

            memset(patternHead, pattern, interval);
            break;
        }
        case PAD_PATTERN:
        {
            interval = static_cast<size_t>(config.PadBytes_);
            
            for (int i = 0; i < 2; ++i)
            {
                unsigned char* pad = block;
                pad += i ? stats.ObjectSize_ : -static_cast<int>(interval);
                memset(pad, pattern, interval);
            }
    
            break;
        }
        default: break;
    }
}
/********************************************************************************//*!
 @brief  Sets the left alignment pattern for a page.

 @param  page
    The page to set the alignment for.
*//*********************************************************************************/
void ObjectAllocator::setLeftAlignment(unsigned char* head)
{
    head += PTR_SIZE;
    memset(head, ALIGN_PATTERN, config.LeftAlignSize_);
}
/********************************************************************************//*!
 @brief  Sets the internal alignment pattern for a block.

 @param  block
    The block to set the alignment for.
*//*********************************************************************************/
void ObjectAllocator::setInterAlignment(unsigned char* block)
{
    block += (stats.ObjectSize_ + static_cast<size_t>(config.PadBytes_));
    memset(block, ALIGN_PATTERN, config.InterAlignSize_);
}
/********************************************************************************//*!
 @brief  Checks for an invalid free. For use in debug mode only. Only call in Free.

 @param  data
    The data to check for.

 @throws OAException, based on the type of invalid free.
*//*********************************************************************************/
void ObjectAllocator::checkForInvalidFree(void* block) const
{
    if (!config.DebugOn_)
        return;

    void* currentPage = nullptr;
    unsigned char* currentBlock = TO_UCHAR_PTR(block);

    checkWithinPages(currentBlock, currentPage);
    checkAlignment(currentPage, currentBlock);

    if (checkCorruption(currentBlock))
    {
        throw OAException{OAException::E_CORRUPTED_BLOCK, "Pad bytes have been overwritten."};
    }

    checkMultipleFree(currentBlock);
}
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
void ObjectAllocator::checkWithinPages(unsigned char* block, void*& currentPage) const
{
    // Check if block is within a pages
    GenericObject* page = PageList_;
    while(page != nullptr)
    {
        if (isInPage(page, block))
        {
            currentPage = page;
            return;
        }

        page = page->Next;
    }
    // return false;
    throw OAException{OAException::E_BAD_BOUNDARY, "Object address is not within a page."};
}
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
void ObjectAllocator::checkAlignment(void* currentPage, unsigned char* block) const
{
    const unsigned char* CP = TO_UCHAR_PTR(currentPage);
    bool isAligned = true;

    if (config.Alignment_ > 1)
    {
        ptrdiff_t offset = block - (CP);
        isAligned = (static_cast<size_t>(offset) % config.Alignment_ == 0);
    }
    else
    {
        ptrdiff_t offset = header(block) - (CP) - PTR_SIZE;
        isAligned = (static_cast<size_t>(offset) % blockSize == 0);
    }

    if (!isAligned)
    {
        throw OAException{OAException::E_BAD_BOUNDARY, "Object that is trying to be freed is misaligned."};
    }
}
/********************************************************************************//*!
 @brief  Checks if left or right padding has been corrupted.

 @param  block
    The block to check for.

 @return True if corrupted.
*//*********************************************************************************/
bool ObjectAllocator::checkCorruption(unsigned char* block) const
{
    bool leftPad = checkPadding(block - config.PadBytes_);
    bool rightPad = checkPadding(block + stats.ObjectSize_);
    return !(leftPad && rightPad);
}
/********************************************************************************//*!
 @brief  Checks if padding has been corrupted.

 @param  padPtr
    The address of the start of the padding in a sequence.

 @return False if corrupted.
*//*********************************************************************************/
bool ObjectAllocator::checkPadding(unsigned char* padPtr) const
{
    for (size_t i = 0; i < config.PadBytes_; ++i)
    { 
        if (padPtr[i] != PAD_PATTERN)
            return false;
    }
    return true;
}
/********************************************************************************//*!
 @brief  Checks if the block has already been freed.

 @param  block
    The block to check for.
    
 @throws OAException for multiple frees.
*//*********************************************************************************/
void ObjectAllocator::checkMultipleFree(unsigned char* block) const
{
    // Compare against all items in the free list
    GenericObject* b = TO_GENERIC_OBJECT_PTR(block);
    GenericObject* fL = FreeList_;

    while(fL != nullptr)
    {
        if (b == fL)
        {
            throw OAException{OAException::E_MULTIPLE_FREE, "Object has already been freed."};
        }
        fL = fL->Next;
    }
}