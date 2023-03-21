/************************************************************************************//*!
\file           BSPTree.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 14, 2022
\brief          Contains the interface for an Binary Space Partition Tree.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
// Project Headers
#include "Plane.h"
#include "Graphics/Drawable.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    struct BSPNode
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

        int                 Index = -1;
        int                 Left;
        int                 Right;

        int                 Height = -1;

        Plane               SplitPlane;
        std::vector<Vec3>   Vertices;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] bool IsLeaf() const;
    };

    class BSPTree
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using DataNode      = std::pair<const BSPNode*, int>;
        using DataNodes     = std::vector<DataNode>;

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr int NULL_NODE          = -1;
        static constexpr int DEFAULT_SIZE       = 65536;
        static constexpr int MIN_THRESHOLD      = 30;
        static constexpr int MAX_THRESHOLD      = 5000;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        BSPTree    (int size = DEFAULT_SIZE);
        BSPTree    (const std::vector<Drawable>& drawables, int size = DEFAULT_SIZE);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] int           GetNodeThreshold    ()  const   { return nodeThreshold; }
        [[nodiscard]] int           GetHeight           ()  const;
        [[nodiscard]] DataNodes     GetDataNodes        ()  const;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetNodeThreshold (int threshold) { nodeThreshold = std::clamp(threshold, MIN_THRESHOLD, MAX_THRESHOLD); }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void    Reset       ();
        void    Build       (const std::vector<Drawable>& drawables);
        void    Serialise   (std::ofstream& file);
        void    Load        (std::ifstream& file);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        enum TriangleState { InFront = 1, Straddling = 0, Behind = -1 };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        int                     root;
        int                     count;
        int                     capacity;
        int                     freeList;

        int                     nodeThreshold;
        int                     maxHeight;

        std::vector<BSPNode>    nodes;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        int                     allocateNode        ();
        void                    deallocateNode      (int index);
        void                    addToFreeList       (int index);

        void                    splitNode           (int parent, const std::vector<Vec3>& triangles);
        static Plane            findSplitPlane      (const std::vector<Vec3>& triangles);
        void                    splitPolygon        (const std::vector<Vec3>& vertices, Plane& p, std::vector<Vec3>& frontPoly, std::vector<Vec3>& backPoly) const;
        static int              classifyPoint       (const Vec3& v, const Plane& p);
        static TriangleState    classifyTriangle    (const Vec3& v0, const Vec3& v1, const Vec3& v2, const Plane& p);
        static void             triangulate         (std::vector<Vec3>& polygon);

        static Mat4             covarianceMatrix    (const std::vector<Vec3>& points);
        static Mat4             computeJacobian     (const Mat4& J, const Mat4& covariance);
        static Vec3             getEigenValueVector (const Mat4& v, const Mat4& covariance);
    };
}

