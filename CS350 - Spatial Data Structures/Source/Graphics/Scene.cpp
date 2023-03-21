/************************************************************************************//*!
\file           Scene.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2022
\brief          Implementation of a Scene to be used with the framework.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// STL Headers
#include <filesystem>
// Primary Header
#include "Graphics/Scene.h"
// Project Headers
#include <fstream>
#include "Geometry/BSphereTree.h"
#include "Graphics/Engine.h"
#include "Geometry/Collision.h"

#define ASSET_PATH "assets/Scenes/"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Public Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    Scene::SpatialPartitions operator|(Scene::SpatialPartitions lhs, Scene::SpatialPartitions rhs)
    {
        const int LHS = static_cast<int>(lhs);
        const int RHS = static_cast<int>(rhs);

        const int RESULT = LHS | RHS;

        return static_cast<Scene::SpatialPartitions>(RESULT);
    }

    void operator|=(Scene::SpatialPartitions& lhs, Scene::SpatialPartitions rhs)
    {
        const int LHS = static_cast<int>(lhs);
        const int RHS = static_cast<int>(rhs);

        lhs = static_cast<Scene::SpatialPartitions>(LHS | RHS);
    }



    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    Scene::Scene(const std::string& sceneName, SpatialPartitions spatialPartitions)
    : name          { sceneName }
    , treeMethod    { Geometry::AABBTree::Method::TopDown }
    , aabbTree      { nullptr }
    , ritterTree    { nullptr }
    , larssonTree   { nullptr }
    , pcaTree       { nullptr }
    , octree        { nullptr }
    , bspTree       { nullptr }
    {
        // Create empty trees based on flags
        const int FLAGS = static_cast<int>(spatialPartitions);

        const int CREATE_AABB_TREE = static_cast<int>(SpatialPartitions::AABBTree);
        if (FLAGS & CREATE_AABB_TREE)
        {
            aabbTree = new Geometry::AABBTree;
        }

        const int CREATE_RITTER_TREE = static_cast<int>(SpatialPartitions::RitterSphereTree);
        if (FLAGS & CREATE_RITTER_TREE)
        {
            ritterTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::Ritter };
        }

        const int CREATE_LARSSON_TREE = static_cast<int>(SpatialPartitions::LarssonSphereTree);
        if (FLAGS & CREATE_LARSSON_TREE)
        {
            larssonTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::Larsson };
        }

        const int CREATE_PCA_TREE = static_cast<int>(SpatialPartitions::PCASphereTree);
        if (FLAGS & CREATE_PCA_TREE)
        {
            pcaTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::PCA };
        }

        const int CREATE_OCTREE = static_cast<int>(SpatialPartitions::Octree);
        if (FLAGS & CREATE_OCTREE)
        {
            octree = new Geometry::Octree;
        }

        const int CREATE_BSPTREE = static_cast<int>(SpatialPartitions::BSPTree);
        if (FLAGS & CREATE_BSPTREE)
        {
            bspTree = new Geometry::BSPTree;
        }
    }

    Scene::Scene(const Scene& rhs)
    : name          { rhs.name }
    , treeMethod    { rhs.GetTreeMethod() }
    , aabbTree      { nullptr }
    , ritterTree    { nullptr }
    , larssonTree   { nullptr }
    , pcaTree       { nullptr }
    , octree        { nullptr }
    , bspTree       { nullptr }
    {
        drawables.resize(rhs.drawables.size());
        std::copy(rhs.drawables.begin(), rhs.drawables.end(), drawables.begin());

        for (auto& light : rhs.lights)
        {
            lights.emplace(light.first, light.second);
        }

        // Create trees
        if (rhs.aabbTree)
        {
            aabbTree = new Geometry::AABBTree;
        }

        if (rhs.ritterTree)
        {
            ritterTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::Ritter };
        }

        if (rhs.larssonTree)
        {
            larssonTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::Larsson };
        }

        if (rhs.pcaTree)
        {
            pcaTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::PCA };
        }

        if (rhs.octree)
        {
            octree = new Geometry::Octree;
        }

        if (rhs.bspTree)
        {
            bspTree = new Geometry::BSPTree;
        }
    }

    Scene& Scene::operator=(const Scene& rhs)
    {
        name        = rhs.name;
        treeMethod  = rhs.treeMethod;

        drawables.resize(rhs.drawables.size());
        std::copy(rhs.drawables.begin(), rhs.drawables.end(), drawables.begin());

        for (auto& light : rhs.lights)
        {
            lights.emplace(light.first, light.second);
        }

        // Delete initial trees
        delete aabbTree;
        delete ritterTree;
        delete larssonTree;
        delete pcaTree;
        delete octree;
        delete bspTree;

        // Create trees
        if (rhs.aabbTree)
        {
            aabbTree = new Geometry::AABBTree;
        }

        if (rhs.ritterTree)
        {
            ritterTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::Ritter };
        }

        if (rhs.larssonTree)
        {
            larssonTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::Larsson };
        }

        if (rhs.pcaTree)
        {
            pcaTree = new Geometry::BSphereTree{ Geometry::BSphereTree::SphereType::PCA };
        }

        if (rhs.octree)
        {
            octree = new Geometry::Octree;
        }

        if (rhs.bspTree)
        {
            bspTree = new Geometry::BSPTree;
        }

        return *this;
    }

    Scene::~Scene()
    {
        delete aabbTree;
        delete ritterTree;
        delete larssonTree;
        delete pcaTree;
        delete octree;
        delete bspTree;
    }


    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    Drawable* Scene::GetDrawable(const std::string& drawableName)
    {
        const size_t ID = Graphics::StringHash(drawableName);
        auto it = std::find_if(drawables.begin(), drawables.end(), [&](const Drawable& drawable)
        {
            return drawable.GetID() == ID;
        });

        if (it == drawables.end())
        {
            Log(LogSeverity::Warning, "Unable to find drawable!");
            return nullptr;
        }

        return &(*it);
    }

    Light* Scene::GetLight(const std::string& lightName)
    {
        const size_t ID = Graphics::StringHash(lightName);
        auto it = lights.find(ID);
        if (it == lights.end())
        {
            Log(LogSeverity::Warning, "Unable to find light!");
            return nullptr;
        }

        return &(it->second);
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Scene::SetTreeMethod(Geometry::AABBTree::Method method)
    {
        treeMethod = method;
    }

    void Scene::SetTreeMethodAndBuild(Geometry::AABBTree::Method method)
    {
        treeMethod = method;
        Build();
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Scene::Build()
    {
        if (aabbTree)
        {
            aabbTree->Reset();
            aabbTree->Build(drawables, treeMethod);
        }

        const auto SPHERE_TREE_METHOD = treeMethod == Geometry::AABBTree::Method::TopDown
                                        ? Geometry::BSphereTree::Method::TopDown
                                        : Geometry::BSphereTree::Method::BottomUp;

        if (ritterTree)
        {
            ritterTree->Reset();
            ritterTree->Build(drawables, SPHERE_TREE_METHOD);
        }

        if (larssonTree)
        {
            larssonTree->Reset();
            larssonTree->Build(drawables, SPHERE_TREE_METHOD);
        }

        if (pcaTree)
        {
            pcaTree->Reset();
            pcaTree->Build(drawables, SPHERE_TREE_METHOD);
        }

        // Specific handling for octree and BSPtree as these are serialised
        if (octree || bspTree)
        {
            if (octree)
                octree->Reset();

            if (bspTree)
                bspTree->Reset();

            // Attempt to load a scene before building
            if (loadScene())
                return;

            if (octree)
                octree->Build(drawables);

            if (bspTree)
                bspTree->Build(drawables);

            serialiseScene();
        }
    }

    void Scene::Rebuild()
    {
        if (aabbTree)
        {
            aabbTree->Reset();
            aabbTree->Build(drawables, treeMethod);
        }

        const auto SPHERE_TREE_METHOD = treeMethod == Geometry::AABBTree::Method::TopDown
                                        ? Geometry::BSphereTree::Method::TopDown
                                        : Geometry::BSphereTree::Method::BottomUp;

        if (ritterTree)
        {
            ritterTree->Reset();
            ritterTree->Build(drawables, SPHERE_TREE_METHOD);
        }

        if (larssonTree)
        {
            larssonTree->Reset();
            larssonTree->Build(drawables, SPHERE_TREE_METHOD);
        }

        if (pcaTree)
        {
            pcaTree->Reset();
            pcaTree->Build(drawables, SPHERE_TREE_METHOD);
        }

        // Forced rebuilding and reserialisation for Octree and BSPTree
        if (octree || bspTree)
        {
            if (octree)
            {
                octree->Reset();
                octree->Build(drawables);
            }

            if (bspTree)
            {
                bspTree->Reset();
                bspTree->Build(drawables);
            }

            serialiseScene();
        }
    }


    void Scene::Clear()
    {
        if (aabbTree)
            aabbTree->Reset();

        if (ritterTree)
            ritterTree->Reset();

        if (larssonTree)
            larssonTree->Reset();

        if (pcaTree)
            pcaTree->Reset();

        if (octree)
            octree->Reset();

        if (bspTree)
            bspTree->Reset();

        drawables.clear();
        lights.clear();
    }

    void Scene::Update()
    {
        for (auto& drawable : drawables)
        {
            drawable.Update();
        }
    }

    Drawable& Scene::AddDrawable(const std::string& drawableName, const Drawable& drawable)
    {
        const size_t ID = Graphics::StringHash(drawableName);

        auto it = std::find_if(drawables.begin(), drawables.end(), [&](Drawable& drawable)
        {
            return drawable.GetID() == ID;
        });
        if (it != drawables.end())
        {
            Log(LogSeverity::Warning, "A Drawable of the same name already exists in the scene!");
            return *it;
        }

        auto& newDrawable = drawables.emplace_back(drawable);
        newDrawable.SetID(drawableName);

        return newDrawable;
    }

    Drawable& Scene::AddDrawable(const std::string& drawableName, Model& model, Material& material, const Transform& transform)
    {
        const size_t ID = Graphics::StringHash(drawableName);

        auto it = std::find_if(drawables.begin(), drawables.end(), [&](Drawable& drawable)
        {
            return drawable.GetID() == ID;
        });
        if (it != drawables.end())
        {
            Log(LogSeverity::Warning, "A Drawable of the same name already exists in the scene!");
            return *it;
        }

        auto& newDrawable = drawables.emplace_back(model, material, transform);
        newDrawable.SetID(drawableName);

        return newDrawable;
    }

    Light& Scene::AddDirectionalLight(const std::string& lightName, const DirectionalLight& light)
    {
        if (lights.size() == Light::MAX_LIGHTS)
        {
            Log(LogSeverity::Error, "Max number of lights in the scene reached!");
            return (--lights.end())->second;
        }
            

        const size_t ID = Graphics::StringHash(lightName);

        auto it = lights.find(ID);
        if (it != lights.end())
        {
            Log(LogSeverity::Warning, "A Light of the same name already exists in the scene!");
            return it->second;
        }

        const auto RETVAL = lights.emplace(ID, light);
        return RETVAL.first->second;
    }

    void Scene::RebuildLarssonTree(Geometry::Sphere::Method larssonMethod)
    {
        if (!larssonTree)
            return;

        for (auto& drawable : drawables)
        {
            drawable.RecomputeLarssonSphere(larssonMethod);
        }

        const Geometry::BSphereTree::Method SPHERE_TREE_METHOD = treeMethod == Geometry::AABBTree::Method::TopDown ?
                                                                 Geometry::BSphereTree::Method::TopDown : Geometry::BSphereTree::Method::BottomUp;

        larssonTree->Reset();
        larssonTree->Build(drawables, SPHERE_TREE_METHOD);
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Scene::serialiseScene()
    {
        if (octree)
        {
            std::string octreeFilePath{ ASSET_PATH };
            octreeFilePath.append(name);
            octreeFilePath.append("_Octree");

            std::ofstream file{ octreeFilePath.c_str(), std::fstream::out | std::fstream::trunc };

            if (!file.is_open())
            {
                Log(LogSeverity::Error, "Failed to open file!");
                return;
            }

            octree->Serialise(file);
            file.close();
        }

        if (bspTree)
        {
            std::string bspTreeFilePath{ ASSET_PATH };
            bspTreeFilePath.append(name);
            bspTreeFilePath.append("_BSPTree");

            std::ofstream file{ bspTreeFilePath.c_str(), std::fstream::out | std::fstream::trunc };

            if (!file.is_open())
            {
                Log(LogSeverity::Error, "Failed to open file!");
                return;
            }

            bspTree->Serialise(file);
            file.close();
        }
    }

    bool Scene::loadScene() const
    {
        if (octree)
        {
            std::string octreeFilePath{ ASSET_PATH };
            octreeFilePath.append(name);
            octreeFilePath.append("_Octree");

            // Check if the file exists
            #ifdef _DEBUG
                Log(LogSeverity::Info, "Checking if file exists: " + octreeFilePath);
            #endif

            const bool FILE_EXISTS = std::filesystem::exists(octreeFilePath);
            if (!FILE_EXISTS)
            {

                return false;
            }
                

            std::ifstream file{ octreeFilePath.c_str(), std::fstream::in };

            if (!file.is_open())
            {
                Log(LogSeverity::Error, "Failed to open file!");
                return false;
            }

            octree->Load(file);
            file.close();
        }

        if (bspTree)
        {
            std::string bspTreeFilePath{ ASSET_PATH };
            bspTreeFilePath.append(name);
            bspTreeFilePath.append("_BSPTree");

            // Check if the file exists
            #ifdef _DEBUG
                Log(LogSeverity::Info, "Checking if file exists: " + bspTreeFilePath);
            #endif

            const bool FILE_EXISTS = std::filesystem::exists(bspTreeFilePath);
            if (!FILE_EXISTS)
            {

                return false;
            }
                

            std::ifstream file{ bspTreeFilePath.c_str(), std::fstream::in };

            if (!file.is_open())
            {
                Log(LogSeverity::Error, "Failed to open file!");
                return false;
            }

            bspTree->Load(file);
            file.close();
        }

        return true;
    }


}
