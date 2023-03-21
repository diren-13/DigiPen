/************************************************************************************//*!
\file           Scene.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2022
\brief          Encapsulation of a View and a Scene to be used with the framework.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <string>
// Project Headers
#include "Camera.h"
#include "Viewport.h"
#include "Drawable.h"
#include "Light.h"
#include "Geometry/AABBTree.h"
#include "Geometry/BSphereTree.h"
#include "Geometry/Octree.h"
#include "Geometry/BSPTree.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    struct View
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Camera*     Camera      = nullptr;
        Viewport*   Viewport    = nullptr;
        bool        active      = true;
    };

    class Scene
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using Drawables = std::vector<Drawable>;
        using Lights    = std::unordered_map<size_t, Light>;

        enum class SpatialPartitions : int
        {
            AABBTree            = 1 
        ,   RitterSphereTree    = 2 
        ,   LarssonSphereTree   = 4 
        ,   PCASphereTree       = 8 
        ,   Octree              = 16
        ,   BSPTree             = 32
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Scene(const std::string& name, SpatialPartitions spatialPartitions);
        Scene(const Scene& rhs);
        Scene& operator=(const Scene& rhs);
        ~Scene();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] Geometry::AABBTree::Method    GetTreeMethod   ()  const   { return treeMethod; }
        [[nodiscard]] const Geometry::AABBTree*     GetAABBTree     ()  const   { return aabbTree; }
        [[nodiscard]] const Geometry::BSphereTree*  GetRitterTree   ()  const   { return ritterTree; }
        [[nodiscard]] const Geometry::BSphereTree*  GetLarssonTree  ()  const   { return larssonTree; }
        [[nodiscard]] const Geometry::BSphereTree*  GetPCATree      ()  const   { return pcaTree; }
        [[nodiscard]] const Geometry::Octree*       GetOctree       ()  const   { return octree; }
        [[nodiscard]] const Geometry::BSPTree*      GetBSPTree      ()  const   { return bspTree; }
        [[nodiscard]] const Drawables&              GetDrawables    ()  const   { return drawables; }
        [[nodiscard]] const Lights&                 GetLights       ()  const   { return lights; }

        [[nodiscard]] Drawable*                     GetDrawable (const std::string& name);
        [[nodiscard]] Light*                        GetLight    (const std::string& name);

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetTreeMethod           (Geometry::AABBTree::Method method);
        void    SetTreeMethodAndBuild   (Geometry::AABBTree::Method method);

        Geometry::Octree*   Octree()    { return octree; }
        Geometry::BSPTree*  BSPTree()   { return bspTree; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void                Build               ();
        void                Rebuild             ();
        void                Clear               ();
        void                Update              ();

        // Adding Drawables
        Drawable&           AddDrawable         (const std::string& name, const Drawable& drawable);
        Drawable&           AddDrawable         (const std::string& name, Model& model, Material& material, const Transform& transform = Transform{});
            
        Light&              AddDirectionalLight (const std::string& name, const DirectionalLight& light);

        // Very Specific
        void                RebuildLarssonTree  (Geometry::Sphere::Method larssonMethod);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::string                 name;

        Geometry::AABBTree::Method  treeMethod;
        Geometry::AABBTree*         aabbTree;
        Geometry::BSphereTree*      ritterTree;
        Geometry::BSphereTree*      larssonTree;
        Geometry::BSphereTree*      pcaTree;

        Geometry::Octree*           octree;
        Geometry::BSPTree*          bspTree;
        

        Drawables                   drawables;
        Lights                      lights;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void serialiseScene ();
        bool loadScene      () const;
    };

    Scene::SpatialPartitions operator|(Scene::SpatialPartitions lhs, Scene::SpatialPartitions rhs);
    void operator|=(Scene::SpatialPartitions& lhs, Scene::SpatialPartitions rhs);
}
