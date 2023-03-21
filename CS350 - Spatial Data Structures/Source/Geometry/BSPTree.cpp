/************************************************************************************//*!
\file           BSPTree.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 14, 2022
\brief          Implementation for an Binary Space Partition Tree
 
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
#include "Geometry/BSPTree.h"
// Project Headers
#include "Tools/Console.h"
#include "Math/CCMath.h"

const std::vector HINT_PLANES =
{
    ClamChowder::Vec3{ 1.0f, 0.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 0.0f, 1.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 1.0f }, 
    ClamChowder::Vec3{ 1.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 1.0f, 0.0f, 1.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 1.0f }
};

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    BSPTree::BSPTree(int size)
    : root          { NULL_NODE }
    , count         { 0 }
    , capacity      { size }
    , freeList      { NULL_NODE }
    , nodeThreshold { 250 }
    , maxHeight     { 10 }
    {
        nodes.resize(static_cast<size_t>(size));
        addToFreeList(0);
    }

    BSPTree::BSPTree(const std::vector<Drawable>& drawables, int size)
    : root          { NULL_NODE }
    , count         { 0 }
    , freeList      { NULL_NODE }
    , nodeThreshold { 250 }
    , maxHeight     { 10 }
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

    int BSPTree::GetHeight() const
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

            const BSPNode& CURRENT_NODE = nodes[INDEX];

            treeHeight = std::max(treeHeight, CURRENT_NODE.Height);

            if (CURRENT_NODE.IsLeaf())
                continue;

            nodeIndices.push(CURRENT_NODE.Left);
            nodeIndices.push(CURRENT_NODE.Right);
        }

        return treeHeight + 1;
    }

    BSPTree::DataNodes BSPTree::GetDataNodes() const
    {
        static DataNodes        dataNodes;
        static std::stack<int>  nodeIndices;

        int nodeCounter = 0;

        dataNodes.clear();

        nodeIndices.push(root);
        while (!nodeIndices.empty())
        {
            const int INDEX = nodeIndices.top();
            nodeIndices.pop();

            if (INDEX == NULL_NODE)
                continue;

            const BSPNode& CURRENT_NODE = nodes[INDEX];
            if (CURRENT_NODE.IsLeaf())
                continue;

            // If children are leaf, put them in
            const int LEFT = CURRENT_NODE.Left;
            const int RIGHT = CURRENT_NODE.Right;
            if (nodes[LEFT].IsLeaf() || nodes[RIGHT].IsLeaf())
            {
                if (nodes[LEFT].IsLeaf())
                {
                    dataNodes.emplace_back(&nodes[LEFT], nodeCounter);
                }

                if (nodes[RIGHT].IsLeaf())
                {
                    dataNodes.emplace_back(&nodes[RIGHT], nodeCounter);
                }

                nodeCounter++;
            }

            nodeIndices.push(CURRENT_NODE.Left);
            nodeIndices.push(CURRENT_NODE.Right);
        }

        return dataNodes;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    bool BSPNode::IsLeaf() const
    {
        return Right == BSPTree::NULL_NODE && Left == BSPTree::NULL_NODE;
    }

    void BSPTree::Reset()
    {
        // Clear all node data
        nodes.clear();
        nodes.resize(static_cast<size_t>(DEFAULT_SIZE));

        std::for_each(nodes.begin(), nodes.end(), [](BSPNode& node)
        {
            node.Parent     = NULL_NODE;
            node.SplitPlane = Plane{};
            node.Height     = NULL_NODE;

            node.Left       = NULL_NODE;
            node.Right      = NULL_NODE;
        });

        count           = 0;
        capacity        = DEFAULT_SIZE;

        addToFreeList(0);
    }

    void BSPTree::Build(const std::vector<Drawable>& drawables)
    {
        if (drawables.empty())
        {
            Log(LogSeverity::Error, "Invalid number of drawables passed into the BSPTree!");
            return;
        }

        root = allocateNode();
        nodes[root].Height = 0;

        std::vector<Vec3> triangles;
        std::for_each(drawables.cbegin(), drawables.cend(), [&](const Drawable& drawable)
        {
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

        const int TRI_COUNT = static_cast<int>(triangles.size() / 3);
        if (TRI_COUNT > nodeThreshold)
        {
            splitNode(root, triangles);
            nodes[root].Vertices.clear();
        }
        else
        {
            nodes[root].Vertices.resize(triangles.size());
            std::copy(triangles.begin(), triangles.end(), nodes[root].Vertices.begin());
        }
    }

    void BSPTree::Serialise(std::ofstream& file)
    {
        if (!file.is_open())
        {
            Log(LogSeverity::Error, "Unable to serialise Octree into an invalid file!");
            return;
        }

        for (int i = 0; i < capacity; ++i)
        {
            // IDX HEIGHT PARENT PN_X PN_Y PN_Z P_D IS_LEAF LEFT RIGHT NUM_VTX X1 Y1 Z1 X2 Y2 Z2...

            if (nodes[i].Index == NULL_NODE)
                continue;

            file    << nodes[i].Index                       << " "
                    << nodes[i].Height                      << " "
                    << nodes[i].Parent                      << " "
                    << nodes[i].SplitPlane.GetNormal().x    << " "
                    << nodes[i].SplitPlane.GetNormal().y    << " "
                    << nodes[i].SplitPlane.GetNormal().z    << " "
                    << nodes[i].SplitPlane.GetDistance()    << " ";

            const bool IS_LEAF = nodes[i].IsLeaf();
            file << (IS_LEAF ? 1 : 0) << " ";

            // Serialise Children
            file << nodes[i].Left << " " << nodes[i].Right << " ";

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

    void BSPTree::Load(std::ifstream& file)
    {
        if (!file.is_open())
        {
            Log(LogSeverity::Error, "Unable to deserialise BSPTree from an invalid file!");
            return;
        }

        std::stringstream ss;
        std::string line;
        while (std::getline(file, line))
        {
            ss << line;

            // IDX HEIGHT PARENT PN_X PN_Y PN_Z P_D IS_LEAF LEFT RIGHT NUM_VTX X1 Y1 Z1 X2 Y2 Z2...
            int nodeIdx;
            ss >> nodeIdx;

            BSPNode& currentNode = nodes[nodeIdx];
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
            Vec3 planeNormal;
            float planeDistance;
            ss >> planeNormal.x >> planeNormal.y >> planeNormal.z >> planeDistance;

            currentNode.SplitPlane.SetNormal(planeNormal);
            currentNode.SplitPlane.SetDistance(planeDistance);

            int isLeaf;
            ss >> isLeaf;

            // Read in children
            ss >> currentNode.Left >> currentNode.Right;

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

    int BSPTree::allocateNode()
    {
        // Expand free list if capacity is reached
        if (freeList == NULL_NODE)
        {
            capacity <<= 1U;
            std::vector<BSPNode> newNodes{ static_cast<size_t>(capacity) };
            std::copy(nodes.begin(), nodes.end(), newNodes.begin());

            // Remake original container of nodes
            nodes.clear();
            nodes.resize(static_cast<size_t>(capacity));
            std::copy(newNodes.begin(), newNodes.end(), nodes.begin());

            addToFreeList(count);
        }

        const int NEW_NODE = freeList;
        freeList = nodes[freeList].Next;

        nodes[NEW_NODE].Index   = NEW_NODE;
        nodes[NEW_NODE].Parent  = NULL_NODE;
        nodes[NEW_NODE].Left    = NULL_NODE;
        nodes[NEW_NODE].Right   = NULL_NODE;
        nodes[NEW_NODE].Height  = 0;

        ++count;
        return NEW_NODE;
    }

    void BSPTree::deallocateNode(int index)
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

    void BSPTree::addToFreeList(int index)
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

    void BSPTree::splitNode(int parent, const std::vector<Vec3>& triangles)
    {
        const int FRONT_IDX   = allocateNode();
        nodes[FRONT_IDX].Height = nodes[parent].Height + 1;
        nodes[FRONT_IDX].Parent = parent;
        nodes[parent].Left = FRONT_IDX;

        int BACK_IDX    = allocateNode();
        nodes[BACK_IDX].Height = nodes[parent].Height + 1;
        nodes[BACK_IDX].Parent = parent;
        nodes[parent].Right = BACK_IDX;

        // Split the points across the plane
        std::vector<Vec3> back;
        std::vector<Vec3> front;

        // Get the splitting plane
        Plane p = findSplitPlane(triangles);

        for (size_t i = 0; i < triangles.size(); i += 3)
        {
            const std::vector VERTICES{ triangles[i], triangles[i + 1], triangles[i + 2] };

            const TriangleState STATE = classifyTriangle(VERTICES[0], VERTICES[1], VERTICES[2], p);

            if (STATE == InFront)
                std::for_each(VERTICES.begin(), VERTICES.end(), [&](const Vec3& v) { front.emplace_back(v); });
            if (STATE == Behind)
                std::for_each(VERTICES.begin(), VERTICES.end(), [&](const Vec3& v) { back.emplace_back(v); });
            if (STATE == Straddling)
            {
                std::vector<Vec3> frontPoly, backPoly;
                splitPolygon(VERTICES, p, frontPoly, backPoly);

                std::for_each(frontPoly.begin(), frontPoly.end(), [&](const Vec3& v) { front.emplace_back(v); });
                std::for_each(backPoly.begin(), backPoly.end(), [&](const Vec3& v) { back.emplace_back(v); });
            }
                
        }

        const int TRI_COUNT_FRONT = static_cast<int>(front.size() / 3U);
        if (TRI_COUNT_FRONT > nodeThreshold && nodes[FRONT_IDX].Height < maxHeight)
        {
            splitNode(FRONT_IDX, front);
        }
        else
        {
            std::for_each(front.begin(), front.end(), [&](const Vec3& v) { nodes[FRONT_IDX].Vertices.emplace_back(v); });
        }

        const int TRI_COUNT_BACK = static_cast<int>(back.size() / 3U);
        if (TRI_COUNT_BACK> nodeThreshold && nodes[FRONT_IDX].Height < maxHeight)
        {
            splitNode(BACK_IDX, back);
        }
        else
        {
            std::for_each(back.begin(), back.end(), [&](const Vec3& v) { nodes[BACK_IDX].Vertices.emplace_back(v); });
        }
    }

    Plane BSPTree::findSplitPlane(const std::vector<Vec3>& triangles)
    {
        const float ONE_OVER_N = 1.0f / static_cast<float>(triangles.size());
        // Compute centroid
        Vec3 center = std::accumulate(triangles.begin(), triangles.end(), Vec3::Zero);
        center *= ONE_OVER_N;

        static constexpr int MAX_ITERATIONS = 50;

        Mat4 A = covarianceMatrix(triangles);

        float prevOffset = 0.0f;

        Mat4 v{ Mat4::Identity };
        Mat4 J{ Mat4::Identity };

        // Find largest off-diagonal absolute element A[p][q]
        for (int n = 0; n < MAX_ITERATIONS; ++n)
        {
            J = computeJacobian(J, A);

            // Cumulate rotations into what will contain the eigenvectors
            v = v * J;
            // Make ’A’ more diagonal, until just eigenvalues remain on diagonal
            A = (J.Transpose() * A) * J;

            // Compute "norm" of off-diagonal elements
            float offset = 0.0f;

            for (int i = 0; i < 3; ++i)
            {
                for (int j = i + 1; j < 3; ++j)
                {
                    offset += A.m[i][j] * A.m[i][j];
                }
            }

            if (n > 2 && offset >= prevOffset)
                break;

            prevOffset = offset;
        }

        const Vec3 E = getEigenValueVector(v, A);

        return Plane{E, center };
    }


    int BSPTree::classifyPoint(const Vec3& v, const Plane& p)
    {
        const float D = p.GetNormal().Dot(v) - p.GetDistance();
        if (abs(D) <= Math::EPSILON)
            return Straddling;

        return (D > 0.0f) ? InFront : Behind;
    }

    BSPTree::TriangleState BSPTree::classifyTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Plane& p)
    {
        const int V0_SIDE = classifyPoint(v0, p);
        const int V1_SIDE = classifyPoint(v1, p);
        const int V2_SIDE = classifyPoint(v2, p);

        // Multiply sides and check against first side sign
        const int V0_SIGN   = V0_SIDE > 0 ? 1 : -1;
        const int P_SIGN    = V0_SIDE * V1_SIDE * V2_SIDE > 0 ? 1 : -1;

        if (V0_SIGN != P_SIGN)
            return Straddling;

        return static_cast<TriangleState>(V0_SIGN);
    }

    void BSPTree::splitPolygon(const std::vector<Vec3>& vertices, Plane& p, std::vector<Vec3>& frontPoly, std::vector<Vec3>& backPoly) const
    {
        // Start from the last point
        Vec3 a = vertices.back();
        auto aSide = classifyPoint(a, p);

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vec3 B = vertices[i];
            const auto B_SIDE = classifyPoint(B, p);

            if (B_SIDE == InFront)
            {
                if (aSide == Behind)
                {
                    Ray ray{ a, B-a };
                    static_cast<void>(p.Raycast(ray));

                    const Vec3 I = Vec3::Lerp(a, B, ray.GetT());
                    frontPoly.emplace_back(I);
                    backPoly.emplace_back(I);
                }

                frontPoly.emplace_back(B);
            }
            else if (B_SIDE == Behind)
            {
                if (aSide == InFront)
                {
                    Ray ray{ a, B-a };
                    static_cast<void>(p.Raycast(ray));

                    const Vec3 I = Vec3::Lerp(a, B, ray.GetT());
                    frontPoly.emplace_back(I);
                    backPoly.emplace_back(I);
                }
                else if (aSide == Straddling)
                {
                    backPoly.emplace_back(a);
                }
                backPoly.emplace_back(B);
            }
            else
            {
                frontPoly.emplace_back(B);
                if (aSide == Behind)
                {
                    backPoly.emplace_back(B);
                }
            }

            a = B;
            aSide = B_SIDE;
        }

        // Triangulate both polygons
        triangulate(frontPoly);
        triangulate(backPoly);
    }

    void BSPTree::triangulate(std::vector<Vec3>& vertices)
    {
        if (vertices.empty())
            return;

        // Make a triangle fan
        std::vector tmp{ vertices };
        vertices.clear();

        const Vec3 CP = tmp.front();
        for (size_t i = 1; i + 1 < tmp.size(); ++i)
        {
            Vec3 v1 = tmp[i];
            Vec3 v2 = tmp[i + 1];

            vertices.emplace_back(CP);
            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
        }
    }

    Mat4 BSPTree::covarianceMatrix(const std::vector<Vec3>& points)
    {
        const float ONE_OVER_N = 1.0f / static_cast<float>(points.size());

        // Compute centroid
        Vec3 center = std::accumulate(points.begin(), points.end(), Vec3::Zero);
        center *= ONE_OVER_N;

        Mat4 covarianceMtx{ Vec3::Zero, Vec3::Zero, Vec3::Zero };

        std::for_each(points.begin(), points.end(), [&](const Vec3& point)->void
        {
            const Vec3 P = point - center;

            covarianceMtx.m[0][0] += (P.x * P.x);
            covarianceMtx.m[1][1] += (P.y * P.y);
            covarianceMtx.m[2][2] += (P.z * P.z);
            covarianceMtx.m[0][1] += (P.x * P.y);
            covarianceMtx.m[0][2] += (P.x * P.z);
            covarianceMtx.m[1][2] += (P.y * P.z);
        });

        covarianceMtx.m[0][0] *= ONE_OVER_N;
        covarianceMtx.m[1][1] *= ONE_OVER_N;
        covarianceMtx.m[2][2] *= ONE_OVER_N;
        covarianceMtx.m[0][1] *= ONE_OVER_N;
        covarianceMtx.m[0][2] *= ONE_OVER_N;
        covarianceMtx.m[1][2] *= ONE_OVER_N;
        covarianceMtx.m[1][0] = covarianceMtx.m[0][1];
        covarianceMtx.m[2][0] = covarianceMtx.m[0][2];
        covarianceMtx.m[2][1] = covarianceMtx.m[1][2];

        return covarianceMtx;
    }

    Mat4 BSPTree::computeJacobian(const Mat4& J, const Mat4& covariance)
    {
        Mat4 outJ = J;

        int p = 0;
        int q = 1;
        for (int j = 0; j < 3; ++j)
        {
            for (int k = j + 1; k < 3; ++k)
            {
                if (std::abs(covariance.m[j][k]) > std::abs(covariance.m[p][q]))
                {
                    p = j;
                    q = k;
                }
            }
        }

        // Compute the Jacobi rotation matrix J(p, q, theta)
        float sin = 0.0f;
        float cos = 0.0f;
        if (std::abs(covariance.m[p][q]) > Math::EPSILON)
        {
            float tan = 0.0f;

            if (const float R = (covariance.m[q][q] - covariance.m[p][p]) / (2.0f * covariance.m[p][q]); R >= 0.0f)
            {
                tan = 1.0f / (R + std::sqrtf(1.0f + (R * R)));
            }
            else
            {
                tan = -1.0f / (-R + std::sqrtf(1.0f + (R * R)));
            }

            cos = 1.0f / std::sqrt(1.0f + (tan * tan));
            sin = tan * cos;
        }
        else
        {
            cos = 1.0f;
            sin = 0.0f;
        }


        for (int i = 0; i < 3; ++i)
        {
            outJ.m[i][0] = outJ.m[i][1] = outJ.m[i][2] = 0.0f;
            outJ.m[i][i] = 1.0f;
        }

        outJ.m[p][p] = cos;  outJ.m[p][q] = sin;
        outJ.m[q][p] = -sin; outJ.m[q][q] = cos;

        return outJ;
    }

    Vec3 BSPTree::getEigenValueVector(const Mat4& v, const Mat4& covariance)
    {
        // Find the component with largest magnitude eigenvalue (largest spread)
        Vec3 e;
        int maxC = 0;
        float maxF, maxE = std::abs(covariance.m[0][0]);
        if ((maxF = std::abs(covariance.m[1][1])) > maxE)
        {
            maxC = 1;
            maxE = maxF;
        }
        if ((maxF = std::abs(covariance.m[2][2])) > maxE)
        {
            maxC = 2;
            maxE = maxF;
        }

        e.x = v.m[0][maxC];
        e.y = v.m[1][maxC];
        e.z = v.m[2][maxC];

        return e;
    }
}