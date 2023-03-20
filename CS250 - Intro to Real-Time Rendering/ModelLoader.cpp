/*+======================================================================================
File:       ModelLoader.cpp

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu
  
Summary:    Methods to load a model.
======================================================================================+*/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "ModelLoader.h"
// External Dependencies

// Project Headers


namespace RTR
{
    ModelLoader::ModelLoader(Device& _device)
    : device { &_device }
    {}

    bool ModelLoader::Load(const std::string& filepath)
    {
        auto Importer = std::make_unique<Assimp::Importer>();

        const aiScene* pScene = Importer->ReadFile
        (   filepath, 
            aiProcess_Triangulate                  
            | aiProcess_LimitBoneWeights            | aiProcess_GenUVCoords                
            | aiProcess_TransformUVCoords           | aiProcess_FindInstances              
            | aiProcess_CalcTangentSpace            | aiProcess_JoinIdenticalVertices      
            | aiProcess_RemoveRedundantMaterials    | aiProcess_FindInvalidData            
            | aiProcess_FlipUVs                     | aiProcess_PreTransformVertices
        );

        // Nodes
        if (!pScene)
            throw std::runtime_error("Failed to load model!");

        auto* root = pScene->mRootNode;
        loadNode(root, pScene);
    }

    void ModelLoader::MapMemory()
    {
        for (auto& mesh : meshes)
        {
            mesh.vertexBuffer->MapMemory(mesh.vertices.data());
            mesh.indexBuffer->MapMemory(mesh.indices.data());
        }
    }


    void ModelLoader::loadNode(aiNode* node, const aiScene* scene)
    {
        for( auto i = 0u, end = node->mNumMeshes; i < end; ++i )
        {
            aiMesh* pMesh = scene->mMeshes[ node->mMeshes[i] ];
            meshes.push_back( loadMesh( *pMesh, *scene ) );
        }

        for( auto i = 0u; i < node->mNumChildren; ++i ) 
        {
            loadNode( node->mChildren[i], scene );
        }
    }

    Mesh ModelLoader::loadMesh(aiMesh& mesh, const aiScene& scene)
    {
        Mesh newMesh;

        uint32_t vtxCount = 0;
        for( auto i = 0u; i < mesh.mNumVertices; ++i ) 
        {
            Geometry::Vertex vtx;

            vtx.pos = glm::vec3{ mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z };
            vtx.uv  = glm::vec2{ mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y };
            vtx.nml = glm::vec3{ mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z };
            vtx.tan = glm::vec3{ mesh.mTangents[i].x, mesh.mTangents[i].y, mesh.mTangents[i].z };

            newMesh.vertices.emplace_back(vtx);
            ++vtxCount;
        }

        // Walk thourgh the faces
        uint32_t idxCount = 0;
        for( auto i = 0u; i < mesh.mNumFaces; ++i ) 
        {
            const auto& Face = mesh.mFaces[i];

            for( auto j = 0u; j < Face.mNumIndices; ++j )
            {
                newMesh.indices.emplace_back(Face.mIndices[j]);
                ++idxCount;
            }
        }

        newMesh.vertexBuffer.reset(new Buffer{ *device, Buffer::Type::Vertex, sizeof(Geometry::Vertex), vtxCount });
        newMesh.indexBuffer.reset(new Buffer{ *device, Buffer::Type::Index, sizeof(uint16_t), idxCount });

        return newMesh;
    }




} // namespace RTR