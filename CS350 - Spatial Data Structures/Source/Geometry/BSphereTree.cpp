/************************************************************************************//*!
\file           BSphereTree.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2022
\brief          Contains the implementation for an Bounding Sphere Tree.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// STL Headers
#include <stack>
// Primary Header
#include "Geometry/BSphereTree.h"
// Project Headers
#include "Tools/Console.h"
#include "Geometry/Plane.h"
#include "Graphics/Renderer.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    BSphereTree::BSphereTree(SphereType t, int size)
    : type          { t }
    , root          { NULL_NODE }
    , count         { 0 }
    , capacity      { size }
    , freeList      { NULL_NODE }
    , maxHeight     { 7 }
    {
        nodes.resize(static_cast<size_t>(size));
        addToFreeList(0);
    }

    BSphereTree::BSphereTree(const std::vector<Drawable>& drawables, Method method, SphereType t, int size)
    : type          { t }
    , root          { NULL_NODE }
    , count         { 0 }
    , freeList      { NULL_NODE }
    , maxHeight     { 7 }
    {
        capacity = std::max(static_cast<int>(drawables.size()), size);
        if (capacity % 1024 != 0)
        {
            const int CLOSEST_MULTIPLE = (capacity / 1024) + 1;
            capacity = CLOSEST_MULTIPLE * 1024;
        }

        Build(drawables, method);
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    int BSphereTree::GetHeight() const
    {
        int treeHeight = 0;
        static std::stack<int>  nodeIndices;

        nodeIndices.push(root);
        while (!nodeIndices.empty())
        {
            const int INDEX = nodeIndices.top();
            if (INDEX == NULL_NODE)
                continue;

            const BSphereTreeNode& CURRENT_NODE = nodes[INDEX];
            nodeIndices.pop();

            treeHeight = std::max(treeHeight, CURRENT_NODE.Height);

            if (CURRENT_NODE.IsLeaf())
                continue;

            nodeIndices.push(CURRENT_NODE.Left);
            nodeIndices.push(CURRENT_NODE.Right);
        }

        return treeHeight + 1;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    bool BSphereTreeNode::IsLeaf() const
    {
        return Right == BSphereTree::NULL_NODE && Left == BSphereTree::NULL_NODE;
    }

    void BSphereTree::Reset()
    {
        // Clear all node data
        nodes.clear();
        nodes.resize(static_cast<size_t>(DEFAULT_SIZE));

        std::for_each(nodes.begin(), nodes.end(), [](BSphereTreeNode& node)
        {
            node.Parent = NULL_NODE;
            node.Sphere = Sphere{};
            node.Data   = nullptr;
            node.Left   = NULL_NODE;
            node.Right  = NULL_NODE;
            node.Height = NULL_NODE;
        });

        count       = 0;
        capacity    = DEFAULT_SIZE;

        addToFreeList(0);
    }

    void BSphereTree::Build(const std::vector<Drawable>& drawables, Method method)
    {
        switch(method)
        {
            case Method::TopDown:
            {
                topDownBuild(drawables);
                break;
            }
            case Method::BottomUp:
            {
                bottomUpBuild(drawables);
                break;
            }
            default: break; // Nothing will be built
        }
    }

    BSphereTree::RenderNodes BSphereTree::GetNodes() const
    {
        static RenderNodes      renderNodes;
        static std::stack<int>  nodeIndices;

        // Stack should be empty by this point
        renderNodes.clear();

        nodeIndices.push(root);
        while (!nodeIndices.empty())
        {
            // Pop the top node
            const int INDEX = nodeIndices.top();
            const BSphereTreeNode& CURRENT_NODE = nodes[INDEX];
            nodeIndices.pop();

            // Skip null nodes
            if (INDEX == NULL_NODE)
                continue;

            renderNodes.emplace_back(CURRENT_NODE.Sphere, CURRENT_NODE.Height);

            if (!CURRENT_NODE.IsLeaf())
            {
                nodeIndices.push(CURRENT_NODE.Left);
                nodeIndices.push(CURRENT_NODE.Right);
            }
        }

        return renderNodes;
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    int BSphereTree::allocateNode()
    {
        // Expand free list if capacity is reached
        if (freeList == NULL_NODE)
        {
            capacity >>= 1U;
            std::vector<BSphereTreeNode> newNodes{ static_cast<size_t>(capacity) };
            std::copy(nodes.begin(), nodes.end(), newNodes.begin());
            nodes.clear(); nodes.resize(static_cast<size_t>(capacity));
            std::copy(newNodes.begin(), newNodes.end(), nodes.begin());

            addToFreeList(count);
        }

        const int NEW_NODE = freeList;
        freeList = nodes[freeList].Next;

        nodes[NEW_NODE].Parent  = NULL_NODE;
        nodes[NEW_NODE].Data    = nullptr;
        nodes[NEW_NODE].Left    = NULL_NODE; 
        nodes[NEW_NODE].Right   = NULL_NODE;
        nodes[NEW_NODE].Height  = 0;

        ++count;
        return NEW_NODE;
    }

    void BSphereTree::deallocateNode(int index)
    {
        if (index < 0 || index >= capacity)
        {
            Log(LogSeverity::Error, "Unable to deallocate invalid node!");
            return;
        }

        nodes[index].Next   = freeList;
        nodes[index].Height = NULL_NODE;
        freeList = index;

        --count;
    }

    void BSphereTree::addToFreeList(int index)
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

    void BSphereTree::topDownBuild(const std::vector<Drawable>& drawables)
    {
        if (drawables.size() <= 0)
        {
            Log(LogSeverity::Error, "Invalid number of drawables passed in! No Tree will be built");
            return;
        }

        // Create root node
        root        = freeList;             // Take first node on free list
        freeList    = nodes[root].Next;

        nodes[root].Parent  = NULL_NODE;    // Root has no parent
        nodes[root].Data    = nullptr;
        nodes[root].Left    = NULL_NODE;
        nodes[root].Right   = NULL_NODE;
        nodes[root].Height  = 0;            // Height at root is 0

        std::vector<Sphere> sceneBoundingSphere;
        for (const auto& drawable : drawables)
        {
            const Sphere* boundingSphere = nullptr;
            switch (type)
            {
                case SphereType::Ritter:    boundingSphere = &drawable.GetRitterSphere();   break;
                case SphereType::Larsson:   boundingSphere = &drawable.GetLarssonSphere();  break;
                case SphereType::PCA:       boundingSphere = &drawable.GetPCASphere();      break;
            }

            if (!boundingSphere)
                continue;

            sceneBoundingSphere.emplace_back(*boundingSphere);
        }
        nodes[root].Sphere = Sphere{ sceneBoundingSphere };

        ++count;

        // Stop recursion
        if (nodes[root].Height == maxHeight || drawables.size() == 1U)
        {
            nodes[root].Data = drawables.data();
            return;
        }

        // Find Split Point
        const Vec3 SPLIT_POINT = findSplitPoint(drawables);
        const TopDownSplitPlane SPLIT_PLANE = getSplitPlane(drawables, SPLIT_POINT);

        // One time copy of the scene for the initial build
        std::vector<const Drawable*> scene;
        for (const auto& drawable : drawables)
        {
            scene.emplace_back(&drawable);
        }

        std::vector<const Drawable*> left;
        std::vector<const Drawable*> right;
        partition(left, right, scene, SPLIT_PLANE, SPLIT_POINT);

        nodes[root].Left    = topDownSplit(left, root);
        nodes[root].Right   = topDownSplit(right, root);

        std::vector<Sphere> childSpheres;
        if (nodes[root].Left != NULL_NODE)
        {
            const int LEFT_INDEX = nodes[root].Left;
            childSpheres.emplace_back(nodes[LEFT_INDEX].Sphere);
        }
        if (nodes[root].Right != NULL_NODE)
        {
            const int RIGHT_INDEX = nodes[root].Right;
            childSpheres.emplace_back(nodes[RIGHT_INDEX].Sphere);
        }

        nodes[root].Sphere = Sphere{ childSpheres };
    }

    int BSphereTree::topDownSplit(std::vector<const Drawable*>& drawables, int parent)
    {
        if (drawables.size() == 0)
            return NULL_NODE;

        const int CURRENT_NODE = allocateNode();
        nodes[CURRENT_NODE].Parent  = parent;
        nodes[CURRENT_NODE].Data    = nullptr;
        nodes[CURRENT_NODE].Height  = nodes[parent].Height + 1;

        std::vector<Sphere> sceneBoundingSphere;
        for (const auto& drawable : drawables)
        {
            const Sphere* boundingSphere = nullptr;
            switch (type)
            {
                case SphereType::Ritter:    boundingSphere = &(drawable->GetRitterSphere());    break;
                case SphereType::Larsson:   boundingSphere = &(drawable->GetLarssonSphere());   break;
                case SphereType::PCA:       boundingSphere = &(drawable->GetPCASphere());       break;
            }

            if (!boundingSphere)
                continue;

            sceneBoundingSphere.emplace_back(*boundingSphere);
        }
        nodes[CURRENT_NODE].Sphere = Sphere{ sceneBoundingSphere };

        if (nodes[CURRENT_NODE].Height == maxHeight || drawables.size() == 1)
        {
            nodes[CURRENT_NODE].Data = *drawables.data();
            return CURRENT_NODE;
        }

        // Use a temporary copy of the drawables to find the required split
        std::vector<Drawable> tmpDrawables;
        for(const auto& drawable : drawables)
        {
            tmpDrawables.emplace_back(*drawable);
        }

        const Vec3 SPLIT_POINT = findSplitPoint(tmpDrawables);
        const TopDownSplitPlane SPLIT_PLANE = getSplitPlane(tmpDrawables, SPLIT_POINT);
        
        std::vector<const Drawable*> left;
        std::vector<const Drawable*> right;
        partition(left, right, drawables, SPLIT_PLANE, SPLIT_POINT);

        nodes[CURRENT_NODE].Left    = topDownSplit(left, CURRENT_NODE);
        nodes[CURRENT_NODE].Right   = topDownSplit(right, CURRENT_NODE);

        std::vector<Sphere> childSpheres;
        if (nodes[CURRENT_NODE].Left != NULL_NODE)
        {
            const int LEFT_INDEX = nodes[CURRENT_NODE].Left;
            childSpheres.emplace_back(nodes[LEFT_INDEX].Sphere);
        }
        if (nodes[CURRENT_NODE].Right != NULL_NODE)
        {
            const int RIGHT_INDEX = nodes[CURRENT_NODE].Right;
            childSpheres.emplace_back(nodes[RIGHT_INDEX].Sphere);
        }

        nodes[CURRENT_NODE].Sphere = Sphere{ childSpheres };
        return CURRENT_NODE;
    }

    Vec3 BSphereTree::findSplitPoint(const std::vector<Drawable>& drawables) const
    {
        const int SIZE = static_cast<int>(drawables.size());
        const int MEDIAN_INDEX = (SIZE % 2 == 0) ? SIZE >> 1 : (((SIZE - 1) >> 1) + ((SIZE + 1) >> 1)) >> 1;

        std::vector<float> xPoints, yPoints, zPoints;
        for (const auto& drawable : drawables)
        {
            const Sphere* boundingSphere = nullptr;
            switch (type)
            {
                case SphereType::Ritter:    boundingSphere = &(drawable.GetRitterSphere());    break;
                case SphereType::Larsson:   boundingSphere = &(drawable.GetLarssonSphere());   break;
                case SphereType::PCA:       boundingSphere = &(drawable.GetPCASphere());       break;
            }

            if (!boundingSphere)
                continue;

            xPoints.emplace_back(boundingSphere->GetCenter().x);
            yPoints.emplace_back(boundingSphere->GetCenter().y);
            zPoints.emplace_back(boundingSphere->GetCenter().z);
        };

        // Sort the points by the distance from the origin (dot product)
        std::sort(xPoints.begin(), xPoints.end());
        std::sort(yPoints.begin(), yPoints.end());
        std::sort(zPoints.begin(), zPoints.end());

        return Vec3{xPoints[MEDIAN_INDEX], yPoints[MEDIAN_INDEX], zPoints[MEDIAN_INDEX]};
    }

    BSphereTree::TopDownSplitPlane BSphereTree::getSplitPlane(const std::vector<Drawable>& drawables, const Vec3& splitPoint)
    {
        std::vector<Sphere> topX, btmX, topY, btmY, topZ, btmZ;
        for (const auto& drawable : drawables)
        {
            const Sphere* boundingSphere = nullptr;
            switch (type)
            {
                case SphereType::Ritter:    boundingSphere = &(drawable.GetRitterSphere());    break;
                case SphereType::Larsson:   boundingSphere = &(drawable.GetLarssonSphere());   break;
                case SphereType::PCA:       boundingSphere = &(drawable.GetPCASphere());       break;
            }

            if (!boundingSphere)
                continue;

            const Vec3& CENTER = boundingSphere->GetCenter();

            // X
            if (CENTER.x < splitPoint.x)
                btmX.emplace_back(*boundingSphere);
            else
                topX.emplace_back(*boundingSphere);

            // Y
            if (CENTER.y < splitPoint.y)
                btmY.emplace_back(*boundingSphere);
            else
                topY.emplace_back(*boundingSphere);

            // Z
            if (CENTER.z < splitPoint.z)
                btmZ.emplace_back(*boundingSphere);
            else
                topZ.emplace_back(*boundingSphere);
        }

        const float TOTAL_HEURISTICS[3]
        {
            Sphere{ topX }.Volume() + Sphere{ btmX }.Volume(),
            Sphere{ topY }.Volume() + Sphere{ btmY }.Volume(),
            Sphere{ topZ }.Volume() + Sphere{ btmZ }.Volume()
        };

        float minCost = std::numeric_limits<float>::max();
        TopDownSplitPlane splitPlane = TopDownSplitPlane::X;

        for (int i = 0; i < ARRAYSIZE(TOTAL_HEURISTICS); ++i)
        {
            if (TOTAL_HEURISTICS[i] < minCost)
            {
                minCost = TOTAL_HEURISTICS[i];

                switch (i)
                {
                    case 0:     splitPlane = TopDownSplitPlane::X;              break;  // redundant. Keep for consistency!
                    case 1:     splitPlane = TopDownSplitPlane::Y;              break;
                    case 2:     splitPlane = TopDownSplitPlane::Z;              break;
                    default:    Log(LogSeverity::Error, "Unhandled Error!");    break;
                }
            }
        }

        return splitPlane;
    }

    void BSphereTree::partition(std::vector<const Drawable*>& left, std::vector<const Drawable*>& right, const std::vector<const Drawable*>& drawables, TopDownSplitPlane splitPlane, const Vec3& splitPoint)
    {
        for (const auto& drawable : drawables)
        {
            const Sphere* boundingSphere = nullptr;
            switch (type)
            {
                case SphereType::Ritter:    boundingSphere = &(drawable->GetRitterSphere());    break;
                case SphereType::Larsson:   boundingSphere = &(drawable->GetLarssonSphere());   break;
                case SphereType::PCA:       boundingSphere = &(drawable->GetPCASphere());       break;
            }

            if (!boundingSphere)
                continue;

            const Vec3& CENTER = boundingSphere->GetCenter();

            switch (splitPlane)
            {
                case TopDownSplitPlane::X:
                {
                    if (CENTER.x < splitPoint.x)
                        right.emplace_back(drawable);
                    else
                        left.emplace_back(drawable);

                    break;
                }
                case TopDownSplitPlane::Y:
                {
                    if (CENTER.y < splitPoint.y)
                        right.emplace_back(drawable);
                    else
                        left.emplace_back(drawable);

                    break;
                }
                case TopDownSplitPlane::Z:
                {
                    if (CENTER.z < splitPoint.z)
                        right.emplace_back(drawable);
                    else
                        left.emplace_back(drawable);

                    break;
                }
            }
        }
    }

    void BSphereTree::bottomUpBuild(const std::vector<Drawable>& drawables)
    {
        if (drawables.size() <= 0)
        {
            Log(LogSeverity::Error, "Invalid number of drawables passed in! No Tree will be built");
            return;
        }

        // Temporary storage of nodes for construction & merging
        std::vector<int> tmpNodes;

        // Store all initial drawables in a leaf node
        for (size_t i = 0; i < drawables.size(); ++i)
        {
            const Sphere* boundingSphere = nullptr;
            switch (type)
            {
                case SphereType::Ritter:    boundingSphere = &(drawables[i].GetRitterSphere());    break;
                case SphereType::Larsson:   boundingSphere = &(drawables[i].GetLarssonSphere());   break;
                case SphereType::PCA:       boundingSphere = &(drawables[i].GetPCASphere());       break;
            }

            if (!boundingSphere)
                continue;

            int newNode = allocateNode();
            nodes[newNode].Sphere   = *boundingSphere;
            nodes[newNode].Data     = drawables.data() + i;
            nodes[newNode].Left     = NULL_NODE;
            nodes[newNode].Right    = NULL_NODE;
            nodes[newNode].Parent   = NULL_NODE;

            tmpNodes.emplace_back(newNode);
        }

        while (tmpNodes.size() > 1U)
        {
            const auto [left, right] = findNodesToMerge(tmpNodes);

            const int NEW_NODE = allocateNode();
            nodes[NEW_NODE].Left    = left;
            nodes[NEW_NODE].Right   = right;

            Sphere combinedSphere{ nodes[left].Sphere }; combinedSphere.Combine(nodes[right].Sphere);
            nodes[NEW_NODE].Sphere  = combinedSphere;
            nodes[NEW_NODE].Height  = NULL_NODE;

            nodes[left].Parent = NEW_NODE;
            nodes[right].Parent = NEW_NODE;

            // Remove merged nodes
            auto Lit = std::find(tmpNodes.begin(), tmpNodes.end(), left);   Lit = tmpNodes.erase(Lit);
            auto Rit = std::find(tmpNodes.begin(), tmpNodes.end(), right);  Rit = tmpNodes.erase(Rit);

            tmpNodes.emplace_back(NEW_NODE);
        }

        root = tmpNodes.front();
        nodes[root].Parent = NULL_NODE;
        nodes[root].Height = 0;

        // Update all the heights in the tree
        std::stack<int> nodeIndices;
        nodeIndices.push(root);

        while (!nodeIndices.empty())
        {
            // Pop the top node
            const int INDEX = nodeIndices.top();
            const BSphereTreeNode& CURRENT_NODE = nodes[INDEX];
            nodeIndices.pop();

            // Skip null nodes
            if (INDEX == NULL_NODE)
                continue;

            // Skip leaves
            if (CURRENT_NODE.IsLeaf())
                continue;

            if (CURRENT_NODE.Left != NULL_NODE)
            {
                nodes[CURRENT_NODE.Left].Height = CURRENT_NODE.Height + 1;
                nodeIndices.push(CURRENT_NODE.Left);
            }
            if (CURRENT_NODE.Right != NULL_NODE)
            {
                nodes[CURRENT_NODE.Right].Height = CURRENT_NODE.Height + 1;
                nodeIndices.push(CURRENT_NODE.Right);
            }
        }
    }

    BSphereTree::NodePair BSphereTree::findNodesToMerge(const std::vector<int>& nodeIndices) const
    {
        NodePair nodePair;

        // Compute best pairs for each node
        std::vector<BestPair> bestPairs;
        for (size_t i = 0; i < nodeIndices.size(); ++i)
        {
            const int CURRENT_INDEX = nodeIndices[i];

            for (size_t j = 1; j < nodeIndices.size(); ++j)
            {
                bestPairs.emplace_back(findBestPair(CURRENT_INDEX, nodeIndices));
            }
        }

        // Pull out smallest cost pair as nodes to merge
        float minCost = std::numeric_limits<float>::max();
        for (size_t i = 0; i < bestPairs.size(); ++i)
        {
            if (bestPairs[i].cost < minCost)
            {
                minCost = bestPairs[i].cost;
                nodePair.left = bestPairs[i].left;
                nodePair.right = bestPairs[i].right;
            }
        }

        return nodePair;
    }

    BSphereTree::BestPair BSphereTree::findBestPair(int targetIndex, const std::vector<int>& nodeIndices) const
    {
        BestPair bestPair{};
        bestPair.left = targetIndex;

        const Sphere& LEFT = nodes[targetIndex].Sphere;

        bestPair.cost = std::numeric_limits<float>::max();
        for(size_t i = 0; i < nodeIndices.size(); ++i)
        {
            if (nodeIndices[i] == targetIndex)
                continue;

            const Sphere& RIGHT = nodes[nodeIndices[i]].Sphere;

            const float NEIGHBOUR_COST = Vec3::DistanceSquared(LEFT.GetCenter(), RIGHT.GetCenter()) * 0.45f;

            Sphere combinedSphere{ LEFT }; combinedSphere.Combine(RIGHT);
            const float COMBINED_VOLUME = combinedSphere.Volume();
            const float SUM_OF_VOLUMES  = LEFT.Volume() + RIGHT.Volume();
            // Take minimum increase in volume
            const float HEURISTIC_COST = COMBINED_VOLUME * 0.45f;
            const float INFLATE_COST = ((COMBINED_VOLUME - SUM_OF_VOLUMES) / SUM_OF_VOLUMES) * 0.1f;

            const float COMBINED_COST = NEIGHBOUR_COST + HEURISTIC_COST + INFLATE_COST;

            if (COMBINED_COST < bestPair.cost)
            {
                bestPair.cost = COMBINED_COST;
                bestPair.right = nodeIndices[i];
            }
        }

        return bestPair;
    }
}