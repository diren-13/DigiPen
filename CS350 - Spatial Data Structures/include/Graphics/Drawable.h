/************************************************************************************//*!
\file           Drawable.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 13, 2022
\brief          Encapsulation of a Drawable Object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Model.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#include "Math/Transform.h"
#include "Geometry/Sphere.h"
#include "Geometry/AABB.h"


namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class Scene;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Drawable
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Drawable    ();
        Drawable    (Model& model, Material& material, const Transform& transform = Transform{});

        Drawable    (const Drawable& rhs);
        Drawable&   operator=(const Drawable& rhs);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] size_t                    GetID           ()  const   { return ID; }
        [[nodiscard]] const Model&              GetModel        ()  const   { return *model; }
        [[nodiscard]] const Material&           GetMaterial     ()  const   { return *material; }
        [[nodiscard]] const Transform&          GetTransform    ()  const   { return tf; }

        [[nodiscard]] const Geometry::AABB&     GetAABB         ()  const   { return aabb; }
        [[nodiscard]] const Geometry::Sphere&   GetRitterSphere ()  const   { return ritterSphere; }
        [[nodiscard]] const Geometry::Sphere&   GetLarssonSphere()  const   { return larssonSphere; }
        [[nodiscard]] const Geometry::Sphere&   GetPCASphere    ()  const   { return pcaSphere; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetID           (const std::string& name);
        void    SetModel        (Model& model);
        void    SetColour       (const Colour& clr);
        void    SetMaterial     (Material& material);
        void    SetPosition     (const Vec3& pos);
        void    SetRotation     (const Vec3& rot);
        void    SetRotation     (float pitch, float yaw, float roll);
        void    SetScale        (const Vec3& scale);
        void    SetTransform    (const Transform& transform);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void    RecomputeLarssonSphere  (Geometry::Sphere::Method larssonMethod);

        void    Update                  ();
        void    Draw                    (Device& device, const Camera& cam, const std::unordered_map<size_t, Light>& lights) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        size_t              ID;         // hashed string using the engine's hash function

        Model*              model;
        Material*           material;

        Transform           tf;

        // Bounding Volumes
        Geometry::AABB      aabb;
        Geometry::Sphere    ritterSphere;
        Geometry::Sphere    larssonSphere;
        Geometry::Sphere    pcaSphere;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void    computeBoundingSpheres  (Geometry::Sphere::Method larssonMethod = Geometry::Sphere::Method::Larsson_14);
        void    computeBoundingBox      ();

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class Scene;
    };


}
