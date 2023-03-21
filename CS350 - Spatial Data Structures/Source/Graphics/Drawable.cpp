/************************************************************************************//*!
\file           Drawable.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 13, 2022
\brief          Implementation of a Drawable Object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Drawable.h"
// Project Header
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Geometry/AABB.h"


namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Drawable::Drawable()
    : ID            { 0 }
    , model         { nullptr }
    , material      { nullptr }
    , aabb          {}
    {}

    Drawable::Drawable(Model& _model, Material& _material, const Transform& _transform)
    : ID            { 0 }
    , model         { &_model }
    , material      { &_material }
    , tf            { _transform }
    , aabb          {}
    {
        computeBoundingBox();
        computeBoundingSpheres();
    }

    Drawable::Drawable(const Drawable& rhs)
    : ID            { rhs.ID }
    , model         { rhs.model }
    , material      { rhs.material }
    , tf            { rhs.tf }
    , aabb          { rhs.aabb }
    , ritterSphere  { rhs.ritterSphere }
    , larssonSphere { rhs.larssonSphere }
    , pcaSphere     { rhs.pcaSphere }
    {}

    Drawable& Drawable::operator=(const Drawable& rhs)
    {
        ID              = rhs.ID;
        model           = rhs.model;
        material        = rhs.material;
        tf              = rhs.tf;
        aabb            = rhs.aabb;
        ritterSphere    = rhs.ritterSphere;
        larssonSphere   = rhs.larssonSphere;
        pcaSphere       = rhs.pcaSphere;

        return *this;
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Drawable::SetID(const std::string& name)
    {
        ID = Graphics::StringHash(name);
    }

    void Drawable::SetModel(Model& _model)
    {
        model = &_model;
        computeBoundingBox();
        computeBoundingSpheres();
    }

    void Drawable::SetColour(const Colour& clr)
    {
        model->SetColour(clr);
    }


    void Drawable::SetMaterial(Material& _material)
    {
        material = &_material;
    }

    void Drawable::SetPosition(const Vec3& pos)
    {
        tf.SetPosition(pos);
    }

    void Drawable::SetRotation(const Vec3& rot)
    {
        tf.SetRotation(rot);
    }

    void Drawable::SetRotation(float pitch, float yaw, float roll)
    {
        tf.SetRotation(pitch, yaw, roll);
    }

    void Drawable::SetScale(const Vec3& scale)
    {
        tf.SetScale(scale);

        // recompute the BVs
        computeBoundingBox();
        computeBoundingSpheres();
    }

    void Drawable::SetTransform(const Transform& transform)
    {
        tf = transform;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Drawable::RecomputeLarssonSphere(Geometry::Sphere::Method larssonMethod)
    {
        std::vector<Vec3> worldVertices;
        for (const auto& mesh : model->GetSubMeshes())
        {
            for (const auto& v : mesh.VertexBuffer.GetVertices())
            {
                // Compute global position
                const Vec3 V_WORLD = Vec3::Transform(v.Position, tf.GetTRS());
                worldVertices.emplace_back(V_WORLD);
            }
        }

        larssonSphere = Geometry::Sphere{ worldVertices, larssonMethod };

        const Mat4 INV_TRS = tf.GetTRS().Invert();
        larssonSphere.SetLocalCenter(Vec3::Transform(larssonSphere.GetCenter(), INV_TRS));
    }


    void Drawable::Update()
    {
        tf.Update();

        // Update Bounding Volumes
        aabb.Update(tf);
        ritterSphere.Update(tf);
        larssonSphere.Update(tf);
        pcaSphere.Update(tf);
    }

    void Drawable::Draw(Device& device, const Camera& cam, const Scene::Lights& lights) const
    {
        Renderer::DefaultVSCBuffer vsCBuffer{};
        vsCBuffer.Colour            = model->GetColour();
        vsCBuffer.World             = tf.GetTRS();
        vsCBuffer.ViewProjection    = cam.GetViewProjectionMatrix();

        Renderer::DefaultPSCBuffer psCBuffer{};
        unsigned directionalLightCount = 0;
        for (const auto& light : lights)
        {
            switch (light.second.GetType())
            {
                case Light::Type::Directional:
                {
                    psCBuffer.DirectionalLights[directionalLightCount++] = *light.second.GetLight<DirectionalLight>();
                    break;
                }
                default: break;
            }
        }
        psCBuffer.DirectionalLightCount = directionalLightCount;
        psCBuffer.CameraPos             = cam.GetPosition();

        material->Set(&vsCBuffer, sizeof(Renderer::DefaultVSCBuffer), &psCBuffer, sizeof(Renderer::DefaultPSCBuffer));

        model->Draw(device);
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Drawable::computeBoundingSpheres(Geometry::Sphere::Method larssonMethod)
    {
        tf.Update();

        std::vector<Vec3> worldVertices;
        for (const auto& mesh : model->GetSubMeshes())
        {
            for (const auto& v : mesh.VertexBuffer.GetVertices())
            {
                // Compute global position
                const Vec3 V_WORLD = Vec3::Transform(v.Position, tf.GetTRS());
                worldVertices.emplace_back(V_WORLD);
            }
        }

        ritterSphere    = Geometry::Sphere{ worldVertices, Geometry::Sphere::Method::Ritter };
        larssonSphere   = Geometry::Sphere{ worldVertices, larssonMethod };
        pcaSphere       = Geometry::Sphere{ worldVertices, Geometry::Sphere::Method::PCA };

        // Set center to local center
        const Mat4 INV_TRS = tf.GetTRS().Invert();
        ritterSphere.SetLocalCenter(Vec3::Transform(ritterSphere.GetCenter(), INV_TRS));
        larssonSphere.SetLocalCenter(Vec3::Transform(larssonSphere.GetCenter(), INV_TRS));
        pcaSphere.SetLocalCenter(Vec3::Transform(pcaSphere.GetCenter(), INV_TRS));
    }

    void Drawable::computeBoundingBox()
    {
        tf.Update();

        std::vector<Vec3> localVertices;
        std::vector<Vec3> worldVertices;
        for (const auto& mesh : model->GetSubMeshes())
        {
            for (const auto& v : mesh.VertexBuffer.GetVertices())
            {
                localVertices.emplace_back(v.Position);

                const Vec3 V_WORLD = Vec3::Transform(v.Position, tf.GetTRS());
                worldVertices.emplace_back(V_WORLD);
            }
        }

        aabb = Geometry::AABB{ worldVertices };
        aabb.SetSupportPoints(localVertices);
    }
}
