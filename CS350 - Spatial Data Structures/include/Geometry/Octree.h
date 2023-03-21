/************************************************************************************//*!
\file           Octree.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 10, 2022
\brief          Interface for an Adaptive Octree. This is specific for holding triangles
                instead of objects.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
#include <unordered_set>
// Project Headers
#include "AABB.h"
#include "Graphics/Drawable.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    struct OctreeNode
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr int NUM_CHILD  = 8;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        union
        {
            int Parent;
            int Next;
        };

        AABB                Aabb;
        std::vector<Vec3>   Vertices;

        int                 Children[NUM_CHILD] = { -1 };

        int                 Index               = -1;
        int                 Height              = -1;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] bool IsLeaf() const;
    };

    class Octree
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using RenderNode    = std::pair<AABB, int>;
        using RenderNodes   = std::vector<RenderNode>;

        using DataNode      = std::pair<const OctreeNode*, int>;
        using DataNodes     = std::vector<DataNode>;

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr int NULL_NODE      = -1;
        static constexpr int DEFAULT_SIZE   = 65536;
        static constexpr int MIN_THRESHOLD  = 30;
        static constexpr int MAX_THRESHOLD  = 5000;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Octree  (int size = DEFAULT_SIZE);
        Octree  (const std::vector<Drawable>& drawables, int size = DEFAULT_SIZE);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] int                               GetNodeThreshold()  const   { return nodeThreshold; }
        [[nodiscard]] int                               GetHeight       ()  const;
        [[nodiscard]] const std::vector<OctreeNode>&    GetNodes        ()  const   { return nodes; }
        [[nodiscard]] RenderNodes                       GetRenderNodes  ()  const;
        [[nodiscard]] DataNodes                         GetDataNodes    ()  const;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetNodeThreshold(int newThreshold) { nodeThreshold = newThreshold; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void    Reset       ();
        void    Build       (const std::vector<Drawable>& drawables);
        void    Serialise   (std::ofstream& file);
        void    Load        (std::ifstream& file);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        int                     root;
        int                     count;
        int                     capacity;
        int                     freeList;

        int                     nodeThreshold;

        std::vector<OctreeNode> nodes;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        int                     allocateNode    ();
        void                    deallocateNode  (int index);
        void                    addToFreeList   (int index);

        void                    splitNode       (int parent, const std::vector<Vec3>& triangles);
        void                    subdivide       (int parent);
        std::unordered_set<int> findOctants     (const std::vector<Vec3>& triPoints, int parent);
    };
}