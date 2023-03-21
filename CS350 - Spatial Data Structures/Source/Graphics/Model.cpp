/************************************************************************************//*!
\file           Model.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation for a Model.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Model.h"
// Assimp Headers
#include "assimp/postprocess.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Model::Model(Device& device, const std::filesystem::path& modelPath)
    : colour    { Colours::White }
    {
        loadModelFromFile(device, modelPath);
    }

    Model::Model(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : colour    { Colours::White }
    {
        auto& mesh = subMeshes.emplace_back();
        mesh.VertexBuffer.Initialise(device, vertices);
        mesh.IndexBuffer.Initialise(device, indices);
    }

    Model::Model(Device& device, const Vertex* vertices, unsigned int numVertices, const uint32_t* indices, unsigned int numIndices)
    : colour    { Colours::White }
    {
        auto& mesh = subMeshes.emplace_back();
        mesh.VertexBuffer.Initialise(device, vertices, numVertices);
        mesh.IndexBuffer.Initialise(device, indices, numIndices);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Mesh::Initialise(Device& device)
    {
        VertexBuffer.Initialise(device);
        IndexBuffer.Initialise(device);
    }

    void Mesh::Initialise(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        VertexBuffer.Initialise(device, vertices);
        IndexBuffer.Initialise(device, indices);
    }

    void Mesh::Initialise(Device& device, const Vertex* vertices, unsigned numVertices, const uint32_t* indices, unsigned numIndices)
    {
        VertexBuffer.Initialise(device, vertices, numVertices);
        IndexBuffer.Initialise(device, indices, numIndices);
    }

    void Mesh::Draw(Device& device) const
    {
        /// TEMP
        const unsigned int STRIDE = sizeof(Vertex);
        const unsigned int OFFSET = 0U;

        device.GetContext()->IASetVertexBuffers(0U, 1U, VertexBuffer.GetBuffer().GetAddressOf(), &STRIDE, &OFFSET);
        device.GetContext()->IASetIndexBuffer(IndexBuffer.GetBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        device.GetContext()->DrawIndexed(IndexBuffer.GetIndexCount(), 0U, 0U);
    }

    void Model::LoadModel(Device& device, const std::filesystem::path& modelPath)
    {
        subMeshes.clear();
        loadModelFromFile(device, modelPath);
    }

    void Model::AddMesh(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        auto& mesh = subMeshes.emplace_back();
        mesh.VertexBuffer.Initialise(device, vertices.data(), static_cast<unsigned int>(vertices.size()));
        mesh.IndexBuffer.Initialise(device, indices.data(), static_cast<unsigned int>(indices.size()));
    }

    void Model::AddMesh(Device& device, const Vertex* vertices, unsigned numVertices, const uint32_t* indices, unsigned numIndices)
    {
        auto& mesh = subMeshes.emplace_back();
        mesh.VertexBuffer.Initialise(device, vertices, numVertices);
        mesh.IndexBuffer.Initialise(device, indices, numIndices);
    }

    void Model::Draw(Device& device) const
    {
        for (const auto& mesh : subMeshes)
        {
            mesh.Draw(device);
        }
    }


    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Model::loadModelFromFile(Device& device, const std::filesystem::path& modelPath)
    {
        Assimp::Importer importer;

        constexpr UINT IMPORT_FLAGS = 
                aiProcess_Triangulate                   // Make sure we get triangles rather than n-vertex polygons
            |   aiProcess_ConvertToLeftHanded           // Left handed conversion for DirectX
            |   aiProcess_FindDegenerates               // Fixes degenerate primitives
            |   aiProcess_JoinIdenticalVertices         // Join identical VERTICES/ optimize indexing
            |   aiProcess_RemoveRedundantMaterials      // Remove redundant materials
            |   aiProcess_FindInvalidData               // Detect invalid model data, such as invalid normal vectors
            |   aiProcess_PreTransformVertices          // Allows for non-scenegraph based rendering
            |   aiProcess_GenNormals;                   // Generates Normals

        const aiScene* SCENE = importer.ReadFile(modelPath.string(), IMPORT_FLAGS);

        if (!SCENE)
        {
            Log(LogSeverity::Error, "Failed to open model file:" + modelPath.string());
            return;
        }

        #ifdef _DEBUG
            Log(LogSeverity::Info, "Loading model: " + modelPath.string() + "...");
        #endif

        const aiNode* ROOT_NODE = SCENE->mRootNode;
        processNode(ROOT_NODE, SCENE);

        // Initialise all meshes
        for (auto& mesh : subMeshes)
        {
            mesh.Initialise(device);
        }

        #ifdef _DEBUG
            Log(LogSeverity::Info, "Loaded Model: " + modelPath.string());
        #endif
    }

    void Model::processNode(const aiNode* node, const aiScene* scene)
    {
        for (unsigned i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* MESH = scene->mMeshes[node->mMeshes[i]];
            subMeshes.emplace_back(loadMesh(MESH));
        }

        for (unsigned i = 0; i < node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::loadMesh(const aiMesh* mesh)
    {
        std::vector<Vertex>     vertices;
        std::vector<uint32_t>   indices;

        for (unsigned i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            vertex.Position.x   = mesh->mVertices[i].x;
            vertex.Position.y   = mesh->mVertices[i].y;
            vertex.Position.z   = mesh->mVertices[i].z;

            vertex.Normal.x     = mesh->mNormals[i].x;
            vertex.Normal.y     = mesh->mNormals[i].y;
            vertex.Normal.z     = mesh->mNormals[i].z;

            vertices.emplace_back(vertex);

            //#ifdef _DEBUG
            //    LogProgress("Loading Vertices: ", static_cast<float>(i) / static_cast<float>(mesh->mNumVertices));
            //#endif
        }

        #ifdef _DEBUG
            LogProgress("Loading Vertices: ", 1.0f);
            std::cout << std::endl;
        #endif

        for (unsigned i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& TRI = mesh->mFaces[i];
            for (unsigned j = 0; j < TRI.mNumIndices; ++j)
            {
                indices.emplace_back(TRI.mIndices[j]);
            }

            //#ifdef _DEBUG
            //        LogProgress("Loading Indices: ", static_cast<float>(i) / static_cast<float>(mesh->mNumFaces));
            //#endif
        }

        #ifdef _DEBUG
            LogProgress("Loading Indices: ", 1.0f);
            std::cout << std::endl;
        #endif

        return Mesh{vertices, indices};
    }




}
