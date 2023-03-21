/************************************************************************************//*!
\file           Model.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a Model
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <filesystem>
// Assimp Headers
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
// Project Headers
#include "Device.h"
#include "Buffer.h"
#include "Vertex.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    struct Mesh
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        VertexBuffer<Vertex>    VertexBuffer;
        IndexBuffer<uint32_t>   IndexBuffer;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void Initialise (Device& device);
        void Initialise (Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        void Initialise (Device& device, const Vertex* vertices, unsigned int numVertices, const uint32_t* indices, unsigned int numIndices);

        void Draw       (Device& device) const;
    };

    class Model
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Model() = default;
        Model   (Device& device, const std::filesystem::path& modelPath);
        Model   (Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        Model   (Device& device, const Vertex* vertices, unsigned int numVertices, const uint32_t* indices, unsigned int numIndices);  

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const Colour&             GetColour   ()  const   { return colour; }
        [[nodiscard]] const std::vector<Mesh>&  GetSubMeshes()  const   { return subMeshes; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetColour(const Colour& clr) { colour = clr; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void LoadModel  (Device& device, const std::filesystem::path& modelPath);

        void AddMesh    (Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        void AddMesh    (Device& device, const Vertex* vertices, unsigned int numVertices, const uint32_t* indices, unsigned int numIndices);

        void Draw       (Device& device) const;
        

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Colour              colour;
        std::vector<Mesh>   subMeshes;
        

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static Mesh     loadMesh           (const aiMesh* mesh);

        void            loadModelFromFile  (Device& device, const std::filesystem::path& modelPath);
        void            processNode        (const aiNode* node, const aiScene* scene);
        
    };
}