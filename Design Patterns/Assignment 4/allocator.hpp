/************************************************************************************//*!
\file           allocator.hpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Nov 9, 2021
\brief          Contains the implementation of the Matrix class.
 
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <iostream>
#include <new>
#include <forward_list>
#include <algorithm>
#include <iterator>
#include <bitset>

/************************************************************************************//*!
\brief      Overload for the global new operator.

\param      size
    The size of the element to allocate

\returns    A void pointer to the allocated type.
*//*************************************************************************************/
void* operator new(size_t size)
{
    // Always allocate at least 1 byte
    const size_t ALLOC_SIZE = size == 0 ? 1 : size;

    // Allocate space
    void* p = malloc(ALLOC_SIZE);
    if (!p)
    {
        throw std::bad_alloc();
    }

    // Allocate message
    std::cout << "  Global allocate " << ALLOC_SIZE << " bytes." << std::endl;
    return p;
}

/************************************************************************************//*!
\brief      Overload for the global delete operator.

\param      p
    The pointer to delete
*//*************************************************************************************/
void operator delete(void *p)
{
    free(p);
    std::cout << "  Global deallocate." << std::endl;
}

/************************************************************************************//*!
\brief      Overload for the global delete operator.

\param      p
    The pointer to delete
\param      size 
    The size of the element to delete
*//*************************************************************************************/
void operator delete(void* p, size_t size)
{
    free(p);
    std::cout << "  Global deallocate. " << size << " bytes." << std::endl;
}

namespace csd2125 
{   
    /*---------------------------------------------------------------------------------*/
    /* Class Definitions                                                               */
    /*---------------------------------------------------------------------------------*/ 
    /********************************************************************************//*!
    \brief      Default constructor for vector.

    \tparam     TDataType
        The data type of each element the allocator holds in its memory pools.
    \tparam     TFlags
        A type which number of bits represents the number of elements each memory pool
        can hold.
    *//*********************************************************************************/
    template <typename TDataType, typename TFlags>
    class allocator 
    {	
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Aliases                                                                */
        /*-----------------------------------------------------------------------------*/ 
        using value_type        = TDataType;
        using size_type         = size_t;
        using reference         = value_type&;
        using const_reference   = const reference;
        using pointer           = value_type*;
        using const_pointer     = const pointer;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        \brief      Allocates elements in the memory pool.

        \param      count
            The number of elements to allocate.
        
        \return     A pointer to the first element allocated.
        *//*****************************************************************************/
        pointer allocate(size_type count);
        /****************************************************************************//*!
        \brief      Deallocates elements in the memory pool.

        \param      p
            A pointer to the element to start deallocating from.
        \param      count
            The number of elements to deallocate.
        *//*****************************************************************************/
        void deallocate(pointer p, size_type count);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr size_type NUM_ELEMS = sizeof(TFlags) * __CHAR_BIT__;

        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        struct Node
        {
            /*-------------------------------------------------------------------------*/
            /* Data Members                                                            */
            /*-------------------------------------------------------------------------*/
            TFlags      allocFlags;             // Indicates which element is allocated. 
            TDataType   elements[NUM_ELEMS];

            /*-------------------------------------------------------------------------*/
            /* Constructor                                                             */
            /*-------------------------------------------------------------------------*/
            /************************************************************************//*!
            \brief      Default constructor for Node.
            *//*************************************************************************/
            Node();

            /*-------------------------------------------------------------------------*/
            /* Function Members                                                        */
            /*-------------------------------------------------------------------------*/
            /************************************************************************//*!
            \brief      Counts the number of remaining allocations in the pool.

            \return     The number of remaining allocations in the pool.
            *//*************************************************************************/
            size_type GetRemainingN();

        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::forward_list<Node> pools;
    };

    struct vector 
    {
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        float x;    // The x coordinate
        float y;    // The y coordiante
        float z;    // The z coordinate
        float w;    // The w(clip) coordinate
    
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        \brief      Default constructor for vector.
        *//*****************************************************************************/
        vector();
        /****************************************************************************//*!
        \brief      Constructor for vector.

        \param      _x
            The x coordinate.
        \param      _y
            The y coordinate.
        \param      _z
            The z coordinate.
        \param      _w
            The w coordinate.
        *//*****************************************************************************/
        vector(float _x, float _y, float _z, float _w);
    
        /*-----------------------------------------------------------------------------*/
        /* Operator Overloads                                                          */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        \brief      The new operator for vector.

        \param      size
            The size of the vector to allocate. This is always 4.

        \returns    A void pointer to the allocated vector.
        *//*****************************************************************************/
        void* operator new(size_t size);
        /****************************************************************************//*!
        \brief      The delete  operator for vector.

        \param      p
            The pointer to delete.
        *//*****************************************************************************/
        void operator delete(void* p);
        /****************************************************************************//*!
        \brief      The new operator for vector.

        \param      p
            The pointer to delete
        \param      size 
            The size of the element to delete
        *//*****************************************************************************/
        void operator delete(void* p, size_t size);
    }; // struct vector

    union vertex 
    {
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vector vertexCoordinates;
        float axisCoordinates[sizeof(vector) / sizeof(float)];

        /****************************************************************************//*!
        \brief      Default constructor for vertex.
        *//*****************************************************************************/
        vertex();
        /****************************************************************************//*!
        \brief      Constructor for vertex.

        \param      _x
            The x coordinate.
        \param      _y
            The y coordinate.
        \param      _z
            The z coordinate.
        \param      _w
            The w coordinate.
        *//*****************************************************************************/
        vertex(float _x, float _y, float _z, float _w);

    }; // union vertex


    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/

    vector::vector()
    : x {0}
    , y {0}
    , z {0}
    , w {0}
    {}

    vector::vector(float _x, float _y, float _z, float _w)
    : x {_x}
    , y {_y}
    , z {_z}
    , w {_w}
    {}

    vertex::vertex()
    : vertexCoordinates()
    {}

    vertex::vertex(float _x, float _y, float _z, float _w)
    : vertexCoordinates(_x, _y, _z, _w)
    {}

    template <typename TDataType, typename TFlags>
    allocator<TDataType, TFlags>::Node::Node()
    : allocFlags {0}
    {}

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    template <typename TDataType, typename TFlags>
    typename allocator<TDataType, TFlags>::pointer allocator<TDataType, TFlags>::allocate(typename allocator<TDataType, TFlags>::size_type count)
    {
        std::cout << "\n  Allocator allocate " << count << " elements. " << std::endl;

        // Handle invalid count
        if (count > NUM_ELEMS) { throw std::bad_alloc(); }

        // Check if a new node is needed.
        // Keep track of starting pos for allocation
        int allocStartPos = -1;

        // Find first node with free bit
        auto targetNode = pools.begin();
        for (; targetNode != pools.end(); ++targetNode)
        {
            size_type remainingN = targetNode->GetRemainingN();
            if (remainingN > 0) 
            { 
                if (count > remainingN)
                {
                    std::cout << "  Did not find space in a pool." << std::endl;
                    std::cout << "  Checking next available pool..." << std::endl;
                    continue;
                }
                else
                {
                    // Fill temp array with bits
                    bool bits[NUM_ELEMS];
                    for (size_type i = 0; i < NUM_ELEMS; ++i) 
                    { 
                        bits[i] = targetNode->allocFlags & (1U << i); 
                    }
                    // std::copy(std::rbegin(bits), std::rend(bits), std::ostream_iterator<bool>(std::cout, ""));
                    // std::cout << std::endl;

                    // Check for contiguous space
                    bool contiguous = false;
                    size_type counter = count;

                    for (size_type i = 0; i < NUM_ELEMS; ++i)
                    {
                        contiguous = (!bits[i]);

                        if (contiguous)
                        {
                            if (allocStartPos < 0) { allocStartPos = static_cast<int>(i); }
                            if (--counter == 0) { break; }
                        }
                        else
                        {
                            allocStartPos = -1;
                            counter = count;
                        }
                    }

                    // If counter cannot reach 0, a new node is needed.
                    // New node allocations always start at 0
                    if (counter != 0)
                    {
                        // Reset alloc start pos
                        allocStartPos = -1;
                        std::cout << "  Did not find space in a pool." << std::endl;
                        std::cout << "  Checking next available pool..." << std::endl;
                        continue;
                    }

                    // Found the correct node
                    break;
                }
            }
        }

        if (targetNode == pools.end())
        {
            std::cout << "  Allocating a new pool." << std::endl;
            pools.emplace_front();
            targetNode = pools.begin(); 
            allocStartPos = 0;
        }
        
        pointer firstAlloc = &(targetNode->elements[allocStartPos]);

        std::cout << "  Found space in a pool for " << count << " elements at index " << allocStartPos << "." << std::endl;
        while (count-- > 0)
        {
            targetNode->allocFlags |= static_cast<TFlags>(1U << allocStartPos++);
        }

        return firstAlloc;
    }

    template <typename TDataType, typename TFlags>
    void allocator<TDataType, TFlags>::deallocate(typename allocator<TDataType, TFlags>::pointer p, typename allocator<TDataType, TFlags>::size_type count)
    {
        // Handle invalid count
        if (count > NUM_ELEMS) { throw std::bad_alloc(); }

        std::cout << "  Allocator deallocate " << count << " elements. " << std::endl;

        // Tracking variables
        auto targetNode     = pools.begin();
        size_type targetPos = -1;           // position to start deallocation from 

        // Find matching address
        for (; targetNode != pools.end(); ++targetNode)
        {
            for (size_type i = 0; i < NUM_ELEMS; ++i)
            {
                // Check each target node for matching element
                // If address matches, assign and break loop
                if (p == &(targetNode->elements[i]))
                {
                    targetPos = i;
                    // std::cout << targetPos << std::endl;
                    break;
                }
            }            

            if (targetPos != static_cast<size_t>(0 - 1)) { break; }

            std::cout << "  Checking next existing pool..." << std::endl;
        }

        // If no address found, throw exception
        if (targetNode == pools.end()) { throw std::bad_alloc(); }

        std::cout << "  Found " << count << " elements in a pool." << std::endl;

        while(count-- > 0)
        {
            targetNode->allocFlags &= (~(1U << targetPos++));
        }

        // 
        pools.remove_if([](Node& node)->bool
        { 
            if (node.allocFlags == 0)
            {
                std::cout << "  Removing an empty pool." << std::endl;
                return true;
            }
            return false;
        });
    }

    template<typename TDataType, typename TFlags>
    typename allocator<TDataType, TFlags>::size_type allocator<TDataType, TFlags>::Node::GetRemainingN()
    {
        size_type remainingN = 0;
        for (size_type i = 0; i < NUM_ELEMS; ++i)
        {
            if (!(allocFlags & (1U >> i))) { ++remainingN; }
        }
        return remainingN;
    }

    /*---------------------------------------------------------------------------------*/
    /* Operator Overloads                                                              */
    /*---------------------------------------------------------------------------------*/

    void* vector::operator new(size_t size)
    {
        std::cout << "  In-class allocate " << size << " bytes." << std::endl;
        return ::operator new(size);
    }

    void vector::operator delete(void* p)
    {
        std::cout << "  In-class deallocate." << std::endl;
        ::operator delete(p);
    }

    void vector::operator delete(void* p, size_t size)
    {
        std::cout << "  In-class deallocate " << size << " bytes." << std::endl;
        ::operator delete(p, size);
    }

} // namespace::csd2125
