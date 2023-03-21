/************************************************************************************//*!
\file           Camera.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation for a 3D Camera.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Camera.h"
// Project Header
#include "Math/CCMath.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Camera::Camera()
    : fieldOfView   { DirectX::XM_PI / 3.0f }
    , nearPlane     { 0.1f }
    , farPlane      { 1000.0f }
    , position      { Vec3::Zero }
    , rotation      { Vec3::Zero }
    , projection    { Mat4::Identity }
    , viewProjection{ Mat4::Identity }
    {
        updateViewMatrix();
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Camera::SetPosition(const Vec3& pos)
    {
        position = pos;
        updateViewMatrix();
        viewProjection = view * projection;
    }

    void Camera::SetRotation(const Vec3& rot)
    {
        rotation = rot;
        updateViewMatrix();
        viewProjection = view * projection;
    }

    void Camera::SetPerspective(float fovRadians, float aspectRatio, float _nearPlane, float _farPlane)
    {
        nearPlane   = _nearPlane;
        farPlane    = _farPlane;
        projection = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearPlane, farPlane);
        viewProjection = view * projection;
    }

    void Camera::SetOrthographic(float _left, float _right, float up, float down, float _nearPlane, float _farPlane)
    {
        nearPlane   = _nearPlane;
        farPlane    = _farPlane;
        projection = DirectX::XMMatrixOrthographicLH(_right - _left, up - down, nearPlane, farPlane);
        viewProjection = view * projection;
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Camera::LookAt(const Vec3& target)
    {
        //Verify that target pos is not the same as camera's pos.
        if (target == position)
            return;

        const Vec3 CAM_TARGET = position - target;

        float pitch = 0.0f;
        if (CAM_TARGET.y != 0.0f)
        {
            const float distance = sqrt(CAM_TARGET.x * CAM_TARGET.x + CAM_TARGET.z * CAM_TARGET.z);
            pitch = atan(CAM_TARGET.y / distance);
        }

        float yaw = 0.0f;
        if (CAM_TARGET.x != 0.0f)
        {
            yaw = atan(CAM_TARGET.x / CAM_TARGET.z);
        }
        if (CAM_TARGET.z > 0)
        {
            yaw += DirectX::XM_PI;
        }

        rotation.x = pitch;
        rotation.y = yaw;

        updateViewMatrix();
        viewProjection = view * projection;
    }

    void Camera::MoveCamera(const Vec3& movementVector)
    {
        position += movementVector;
        updateViewMatrix();
        viewProjection = view * projection;
    }

    void Camera::RotateCamera(const Vec3& rotationVector)
    {
        rotation += rotationVector;
        updateViewMatrix();
        viewProjection = view * projection;
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Camera::updateViewMatrix()
    {
        Mat4 rotationMatrix = Mat4::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);

        Vec4 f  { DEFAULT_FORWARD.x,    DEFAULT_FORWARD.y,  DEFAULT_FORWARD.z,  0.0f };
        Vec4 r  { DEFAULT_RIGHT.x,      DEFAULT_RIGHT.y,    DEFAULT_RIGHT.z,    0.0f };
        Vec4 b  { DEFAULT_BACK.x,       DEFAULT_BACK.y,     DEFAULT_BACK.z,     0.0f };
        Vec4 l  { DEFAULT_LEFT.x,       DEFAULT_LEFT.y,     DEFAULT_LEFT.z,     0.0f };

        const Vec4 UP       { DEFAULT_UP.x, DEFAULT_UP.y, DEFAULT_UP.z, 0.0f };
        const Vec4 POS      { position.x, position.y, position.z, 0.0f }; 

        const Vec4 TARGET   = Vec4::Transform(f, rotationMatrix) + POS;
        const Vec4 CAM_UP   = Vec4::Transform(UP, rotationMatrix);

        rotationMatrix = Mat4::CreateFromYawPitchRoll(rotation.y, 0.0f, 0.0f);
        f = Vec4::Transform(f, rotationMatrix); forward = Vec3{ f.x, f.y, f.z };
        r = Vec4::Transform(r, rotationMatrix); right   = Vec3{ r.x, r.y, r.z };
        b = Vec4::Transform(b, rotationMatrix); back    = Vec3{ b.x, b.y, b.z };
        l = Vec4::Transform(l, rotationMatrix); left    = Vec3{ l.x, l.y, l.z };

        //Rebuild view matrix
        view = DirectX::XMMatrixLookAtLH(POS, TARGET, CAM_UP);
    }

}