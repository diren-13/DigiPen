/************************************************************************************//*!
\file           Octree.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           JUly 10, 2022
\brief          Implementation for an Adaptive Octree
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// STL Headers
#include <stack>
#include <fstream>
// Primary Header
#include "Geometry/Octree.h"
// Project Headers
#include "Geometry/Plane.h"
#include "Tools/Console.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Octree::Octree(int size)
    : root          { NULL_NODE }
    , count         { 0 }
    , capacity      { size }
    , freeList      { NULL_NODE }
    , nodeThreshold { 200 }
    {
        nodes.resize(static_cast<size_t>(size));
        addToFreeList(0);
    }

    Octree::Octree(const std::vector<Drawable>& drawables, int size)
    : root          { NULL_NODE }
    , count         { 0 }
    , freeList      { NULL_NODE }
    , nodeThreshold { 200 }
    {
        capacity = std::max(static_cast<int>(drawables.size()), size);
        if (capacity % DEFAULT_SIZE != 0)
        {
            const int CLOSEST_MULTIPLE = (capacity / DEFAULT_SIZE) + 1;
            capacity = CLOSEST_MULTIPLE * DEFAULT_SIZE;
        }

        Build(drawables);
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    int Octree::GetHeight() const
    {
        int                    treeHeight = 0;
        static std::stack<int> nodeIndices;

        nodeIndices.push(root);
        while (!nodeIndices.empty())
        {
            const int INDEX = nodeIndices.top();
            nodeIndices.pop();

            if (INDEX == NULL_NODE)
                continue;

            const OctreeNode& CURRENT_NODE = nodes[INDEX];

            treeHeight = std::max(treeHeight, CURRENT_NODE.Height);

            if (!CURRENT_NODE.IsLeaf())
            {
                for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
                {
                    nodeIndices.push(CURRENT_NODE.Children[i]);
                }
            }
        }

        return treeHeight + 1;
    }

    Octree::RenderNodes Octree::GetRenderNodes() const
    {
        static RenderNodes      renderNodes;
        static std::stack<int>  nodeIndices;

        renderNodes.clear();

        nodeIndices.push(root);
        while (!nodeIndices.empty())
        {
            const int INDEX = nodeIndices.top();
            nodeIndices.pop();

            if (INDEX == NULL_NODE)
                continue;

            const OctreeNode& CURRENT_NODE = nodes[INDEX];

            renderNodes.emplace_back(CURRENT_NODE.Aabb, CURRENT_NODE.Height);
            if (!CURRENT_NODE.IsLeaf())
            {
                for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
                {
                    nodeIndices.push(CURRENT_NODE.Children[i]);
                }
            }
        }

        return renderNodes;
    }

    Octree::DataNodes Octree::GetDataNodes() const
    {
        static DataNodes        dataNodes;
        static std::stack<int>  nodeIndices;


        dataNodes.clear();

        nodeIndices.push(root);
        while (!nodeIndices.empty())
        {
            const int INDEX = nodeIndices.top();
            nodeIndices.pop();

            if (INDEX == NULL_NODE)
                continue;

            const OctreeNode& CURRENT_NODE = nodes[INDEX];

            if (CURRENT_NODE.IsLeaf())
            {
                dataNodes.emplace_back(&CURRENT_NODE, CURRENT_NODE.Height);
                continue;
            }

            // Non-leaf
            for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
            {
                nodeIndices.push(CURRENT_NODE.Children[i]);
            }
        }

        return dataNodes;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    bool OctreeNode::IsLeaf() const
    {
        for (const auto idx : Children)
        {
            if (idx != Octree::NULL_NODE)
                return false;
        }

        return true;
    }

    void Octree::Reset()
    {
        // Clear all node data
        nodes.clear();
        nodes.resize(static_cast<size_t>(DEFAULT_SIZE));

        std::for_each(nodes.begin(), nodes.end(), [](OctreeNode& node)
        {
            node.Parent = NULL_NODE;
            node.Aabb   = AABB{};
            node.Height = NULL_NODE;

            for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
            {
                node.Children[i] = NULL_NODE;
            }
        });

        count           = 0;
        capacity        = DEFAULT_SIZE;

        addToFreeList(0);
    }

    void Octree::Build(const std::vector<Drawable>& drawables)
    {
        if (drawables.empty())
        {
            Log(LogSeverity::Error, "Invalid number of drawables passed into the Octree!");
            return;
        }

        root = allocateNode();
        nodes[root].Height = 0;
        nodes[root].Aabb.SetMin(drawables.front().GetAABB().GetMin());
        nodes[root].Aabb.SetMax(drawables.front().GetAABB().GetMax());
        

        // Compute initial AABB around entire scene and get all triangle points
        std::vector<Vec3> triangles;
        std::for_each(drawables.cbegin(), drawables.cend(), [&](const Drawable& drawable)
        {
            nodes[root].Aabb.Combine(drawable.GetAABB());
            const Mat4& TRS = drawable.GetTransform().GetTRS();

            // Get triangles based on indices
            for (const auto& mesh : drawable.GetModel().GetSubMeshes())
            {
                const auto INDICES = mesh.IndexBuffer.GetIndices();
                for (size_t i = 0; i < INDICES.size(); i += 3)
                {
                    const auto I1 = INDICES[i];
                    const auto I2 = INDICES[i + 1];
                    const auto I3 = INDICES[i + 2];

                    const Vec3& V1 = mesh.VertexBuffer.GetVertices()[I1].Position;
                    const Vec3& V2 = mesh.VertexBuffer.GetVertices()[I2].Position;
                    const Vec3& V3 = mesh.VertexBuffer.GetVertices()[I3].Position;

                    triangles.emplace_back(Vec3::Transform(V1, TRS));
                    triangles.emplace_back(Vec3::Transform(V2, TRS));
                    triangles.emplace_back(Vec3::Transform(V3, TRS));
                }
            }
        });

        if (triangles.size() % 3 != 0)
        {
            Log(LogSeverity::Error, "Invalid number of triangles passed in!");
            return;
        }

        const int TRI_COUNT = static_cast<int>(triangles.size() / 3U);
        if (TRI_COUNT > nodeThreshold)
        {
            splitNode(root, triangles);
        }
        else
        {
            std::for_each(triangles.begin(), triangles.end(), [&](const Vec3& v){ nodes[root].Vertices.emplace_back(v); });
        }
    }

    void Octree::Serialise(std::ofstream& file)
    {
        if (!file.is_open())
        {
            Log(LogSeverity::Error, "Unable to serialise Octree into an invalid file!");
            return;
        }

        for (int i = 0; i < capacity; ++i)
        {
            // IDX HEIGHT PARENT MIN_X MIN_Y MIN_Z MAX_X MAX_Y MAX_Z IS_LEAF C0 C1 C2 C3 C4 C5 C6 C7 C8 NUM_VTX X1 Y1 Z1 X2 Y2 Z2...

            if (nodes[i].Index == NULL_NODE)
                continue;

            file    << nodes[i].Index           << " "
                    << nodes[i].Height          << " "
                    << nodes[i].Parent          << " "
                    << nodes[i].Aabb.GetMin().x << " "
                    << nodes[i].Aabb.GetMin().y << " "
                    << nodes[i].Aabb.GetMin().z << " "
                    << nodes[i].Aabb.GetMax().x << " "
                    << nodes[i].Aabb.GetMax().y << " "
                    << nodes[i].Aabb.GetMax().z << " ";

            const bool IS_LEAF = nodes[i].IsLeaf();
            file << (IS_LEAF ? 1 : 0) << " ";

            // Serialise Children
            std::for_each_n(std::begin(nodes[i].Children), OctreeNode::NUM_CHILD, [&](int childIdx) { file << childIdx << " "; });

            if (!IS_LEAF)
            {
                file << std::endl;
                continue;
            }

            // Serialise Vertices
            file << nodes[i].Vertices.size() << " ";
            std::for_each(nodes[i].Vertices.begin(), nodes[i].Vertices.end(), [&](const Vec3& v)
            {
                    file << v.x << " " << v.y << " " << v.z << " ";
            });

            file << std::endl;
        }
    }

    void Octree::Load(std::ifstream& file)
    {
        if (!file.is_open())
        {
            Log(LogSeverity::Error, "Unable to deserialise Octree from an invalid file!");
            return;
        }

        std::stringstream ss;
        std::string line;
        while (std::getline(file, line))
        {
            ss << line;

            // IDX HEIGHT PARENT MIN_X MIN_Y MIN_Z MAX_X MAX_Y MAX_Z IS_LEAF C0 C1 C2 C3 C4 C5 C6 C7 C8 NUM_VTX X1 Y1 Z1 X2 Y2 Z2...
            int nodeIdx;
            ss >> nodeIdx;

            OctreeNode& currentNode = nodes[nodeIdx];
            currentNode.Index = nodeIdx;

            // root is always 0
            // in instances where a tree needs to be balanced, this may not be the case
            // TN for future implementations
            if (root == NULL_NODE)
            {
                root = nodeIdx;
            }

            ss >> currentNode.Height >> currentNode.Parent;

            // set node AABB
            Vec3 aabbMin, aabbMax;
            ss >> aabbMin.x >> aabbMin.y >> aabbMin.z;
            ss >> aabbMax.x >> aabbMax.y >> aabbMax.z;

            currentNode.Aabb.SetMin(aabbMin);
            currentNode.Aabb.SetMax(aabbMax);

            int isLeaf;
            ss >> isLeaf;

            // Read in children
            for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
            {
                ss >> currentNode.Children[i];
            }

            // Skip non-leaf nodes (data ends at IS_LEAF)
            if (!isLeaf)
            {
                ss.clear();
                continue;
            }

            // Add all vertices
            size_t numVtx;
            ss >> numVtx;
            while(numVtx--)
            {
                Vec3 newVertex;
                ss >> newVertex.x >> newVertex.y >> newVertex.z;
                currentNode.Vertices.emplace_back(newVertex);
            }

            // End of line
            ss.clear();
        }
        
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    int Octree::allocateNode()
    {
        // Expand free list if capacity is reached
        if (freeList == NULL_NODE)
        {
            capacity <<= 1U;
            std::vector<OctreeNode> newNodes{ static_cast<size_t>(capacity) };
            std::copy(nodes.begin(), nodes.end(), newNodes.begin());

            nodes.clear();
            nodes.resize(static_cast<size_t>(capacity));
            std::copy(newNodes.begin(), newNodes.end(), nodes.begin());

            addToFreeList(count);
        }

        const int NEW_NODE = freeList;
        freeList = nodes[freeList].Next;

        nodes[NEW_NODE].Index   = NEW_NODE;
        nodes[NEW_NODE].Parent  = NULL_NODE;
        nodes[NEW_NODE].Height  = 0;

        for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
        {
            nodes[NEW_NODE].Children[i] = NULL_NODE;
        }

        ++count;
        return NEW_NODE;
    }

    void Octree::deallocateNode(int index)
    {
        if (index < 0 || index >= capacity)
        {
            Log(LogSeverity::Error, "Unable to deallocate invalid node!");
            return;
        }

        nodes[index].Index  = NULL_NODE;
        nodes[index].Next   = freeList;
        nodes[index].Height = NULL_NODE;
        freeList = index;

        --count;
    }

    void Octree::addToFreeList(int index)
    {
        // Populate free list
        for (int i = index; i < capacity - 1; ++i)
        {
            nodes[i].Next   = i + 1;
            nodes[i].Height = NULL_NODE;
        }

        nodes[static_cast<size_t>(capacity) - 1U].Next    = NULL_NODE;
        nodes[static_cast<size_t>(capacity) - 1U].Height  = NULL_NODE;

        // Next available
        freeList = index;
    }

    void Octree::splitNode(int parent, const std::vector<Vec3>& triangles)
    {
        // Allocate new nodes
        subdivide(parent);

        std::vector<Vec3> vertices[OctreeNode::NUM_CHILD];

        for (size_t i = 0U; i < triangles.size(); i += 3)
        {
            std::vector triPoints { triangles[i], triangles[i + 1], triangles[i + 2] };
            const auto OCTANTS = findOctants(triPoints, parent);
            for (const auto o : OCTANTS)
            {
                const int CHILD_IDX = nodes[parent].Children[o];
                for (const auto& v : triPoints)
                {
                    vertices[o].emplace_back(v);
                }
            }
        }

        for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
        {
            if (vertices[i].empty())
                continue;

            const int CHILD_IDX = nodes[parent].Children[i];

            const int TRI_COUNT = static_cast<int>(vertices[i].size() / 3U);
            if (TRI_COUNT < nodeThreshold)
            {
                std::for_each(vertices[i].begin(), vertices[i].end(), [&](const Vec3& v)
                {
                    
                    nodes[CHILD_IDX].Vertices.emplace_back(v);
                });
            }
            else
            {
                splitNode(CHILD_IDX, vertices[i]);
            }
        }
    }

    void Octree::subdivide(int parent)
    {
        const Vec3  NEW_HALF_EXTENTS    = nodes[parent].Aabb.GetHalfExtents() * 0.5f;
        const Vec3& CURRENT_CENTER      = nodes[parent].Aabb.GetCenter();
        for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
        {
            const int NEW_CHILD_IDX = allocateNode();

            nodes[parent].Children[i] = NEW_CHILD_IDX;
            OctreeNode& newChild = nodes[NEW_CHILD_IDX];

            newChild.Parent = parent;
            newChild.Height = nodes[parent].Height + 1;
            newChild.Aabb.SetHalfExtents(NEW_HALF_EXTENTS);

            // Set all children to null
            for (auto& child : newChild.Children)
            {
                child = NULL_NODE;
            }

            // Set centers
            switch (i)
            {
                case 0: // -x -y +z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x -= NEW_HALF_EXTENTS.x;
                    newCenter.y -= NEW_HALF_EXTENTS.y;
                    newCenter.z += NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 1: // +x -y +z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x += NEW_HALF_EXTENTS.x;
                    newCenter.y -= NEW_HALF_EXTENTS.y;
                    newCenter.z += NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 2: // -x +y +z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x -= NEW_HALF_EXTENTS.x;
                    newCenter.y += NEW_HALF_EXTENTS.y;
                    newCenter.z += NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 3: // +x +y +z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x += NEW_HALF_EXTENTS.x;
                    newCenter.y += NEW_HALF_EXTENTS.y;
                    newCenter.z += NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 4: // -x -y -z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x -= NEW_HALF_EXTENTS.x;
                    newCenter.y -= NEW_HALF_EXTENTS.y;
                    newCenter.z -= NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 5: // +x -y -z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x += NEW_HALF_EXTENTS.x;
                    newCenter.y -= NEW_HALF_EXTENTS.y;
                    newCenter.z -= NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 6: // -x +y -z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x -= NEW_HALF_EXTENTS.x;
                    newCenter.y += NEW_HALF_EXTENTS.y;
                    newCenter.z -= NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }
                case 7: // +x +y -z
                {
                    Vec3 newCenter = CURRENT_CENTER;
                    newCenter.x += NEW_HALF_EXTENTS.x;
                    newCenter.y += NEW_HALF_EXTENTS.y;
                    newCenter.z -= NEW_HALF_EXTENTS.z;
                    newChild.Aabb.SetCenter(newCenter);

                    break;
                }

                default: break;
            }
        }
    }

    std::unordered_set<int> Octree::findOctants(const std::vector<Vec3>& triPoints, int parent)
    {
        std::unordered_set<int> octants;

        for (const auto& V : triPoints)
        {
            for (int i = 0; i < OctreeNode::NUM_CHILD; ++i)
            {
                const int CHILD_IDX = nodes[parent].Children[i];
                if (nodes[CHILD_IDX].Aabb.TestPoint(V))
                {
                    octants.emplace(i);
                    break;
                }
            }
        }

        return octants;
    }
}
