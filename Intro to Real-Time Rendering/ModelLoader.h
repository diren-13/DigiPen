/*+======================================================================================
File:       ModelLoader.h

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu
  
Summary:    Methods to load a model.
======================================================================================+*/

#pragma once

// Standard Libraries
#include <string>
#include <vector>
// External Dependencies
#include "Importer.hpp"
#include "scene.h"
#include "mesh.h"
#include "postprocess.h"
// Project Headers
#include "core/Device.h"
#include "core/Buffer.h"
#include "geometry/Vertex.h"


namespace RTR
{
    struct Mesh
    {
    public:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;

        std::vector<Geometry::Vertex>   vertices;
        std::vector<uint16_t>           indices;
    };

    class ModelLoader
    {
    public:
        ModelLoader(Device& device);

        bool Load(const std::string& filepath);
        void MapMemory();

        const std::vector<Mesh>& GetMeshes() const { return meshes; }

    private:
        Device*                         device;
        std::vector<Mesh>               meshes;

        void loadNode(aiNode* node, const aiScene* scene);
        Mesh loadMesh(aiMesh& mesh, const aiScene& scene);
    };
}
