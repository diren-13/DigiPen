/************************************************************************************//*!
\file           BSphereTree.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2022
\brief          Contains the interface for an Bounding Sphere Tree.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
// Project Headers
#include "AABB.h"
#include "Graphics/Drawable.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    struct BSphereTreeNode
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        union
        {
            int Parent;
            int Next;
        };

        Sphere          Sphere;
        const Drawable* Data = nullptr;

        int             Left;
        int             Right;

        int             Height = -1;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] bool IsLeaf() const;
    };

    class BSphereTree
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using RenderNode    = std::pair<Sphere, int>;
        using RenderNodes   = std::vector<RenderNode>;

        enum class Method
        {
            TopDown,
            BottomUp
        };

        enum class SphereType
        {
            Ritter
        ,   Larsson
        ,   PCA
        };

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr int NULL_NODE      = -1;
        static constexpr int DEFAULT_SIZE   = 1024;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        BSphereTree (SphereType type, int size = DEFAULT_SIZE);
        BSphereTree (const std::vector<Drawable>& drawables, Method method, SphereType type, int size = DEFAULT_SIZE);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] int           GetMaxHeight()  const   { return maxHeight; }
        [[nodiscard]] RenderNodes   GetNodes    ()  const;

        [[nodiscard]] int           GetHeight   ()  const;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetMaxHeight(int height)    { maxHeight = height; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void        Reset       ();
        void        Build       (const std::vector<Drawable>& drawables, Method method);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        enum class TopDownSplitPlane { X = 0, Y = 1, Z = 2};

        struct NodePair { int left = NULL_NODE; int right = NULL_NODE; };
        struct BestPair { float cost = 0.0f; int left = NULL_NODE; int right = NULL_NODE; };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        SphereType                  type;

        int                         root;
        int                         count;
        int                         capacity;
        int                         freeList;

        int                         maxHeight;

        std::vector<BSphereTreeNode>nodes;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        int     allocateNode    ();
        void    deallocateNode  (int index);
        void    addToFreeList   (int index);

        // Top Down Build
        void                topDownBuild    (const std::vector<Drawable>& drawables);
        Vec3                findSplitPoint  (const std::vector<Drawable>& drawables) const;
        TopDownSplitPlane   getSplitPlane   (const std::vector<Drawable>& drawables, const Vec3& splitPoint);
        void                partition       (std::vector<const Drawable*>& left, std::vector<const Drawable*>& right, const std::vector<const Drawable*>& drawables, TopDownSplitPlane splitPlane, const Vec3& splitPoint);
        int                 topDownSplit    (std::vector<const Drawable*>& drawables, int parent);

        // Bottom Up Build
        void                bottomUpBuild   (const std::vector<Drawable>& drawables);
        NodePair            findNodesToMerge(const std::vector<int>& nodeIndices) const;
        BestPair            findBestPair    (int targetIndex, const std::vector<int>& nodeIndices) const;
        
    };
}

