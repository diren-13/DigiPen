/************************************************************************************//*!
\file           Camera.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a 3D Camera.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Math/MathDefines.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class Camera
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr Vec3   DEFAULT_UP      { 0.0f, 1.0f, 0.0f }; 
        static constexpr Vec3   DEFAULT_FORWARD { 0.0f, 0.0f, 1.0f }; 
        static constexpr Vec3   DEFAULT_RIGHT   { 1.0f, 0.0f, 0.0f }; 
        static constexpr Vec3   DEFAULT_BACK    { 0.0f, 0.0f,-1.0f }; 
        static constexpr Vec3   DEFAULT_LEFT    {-1.0f, 0.0f, 0.0f }; 

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Camera();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] float         GetFieldOfView          ()  const   { return fieldOfView; }
        [[nodiscard]] float         GetNearPlane            ()  const   { return nearPlane; }
        [[nodiscard]] float         GetFarPlane             ()  const   { return farPlane; }
        [[nodiscard]] const Vec3&   GetPosition             ()  const   { return position; }
        [[nodiscard]] const Vec3&   GetRotation             ()  const   { return rotation; }
        [[nodiscard]] const Vec3&   GetForward              ()  const   { return forward; }
        [[nodiscard]] const Vec3&   GetRight                ()  const   { return right; }
        [[nodiscard]] const Vec3&   GetBack                 ()  const   { return back; }
        [[nodiscard]] const Vec3&   GetLeft                 ()  const   { return left; }

        [[nodiscard]] const Mat4&   GetViewMatrix           ()  const   { return view; }
        [[nodiscard]] const Mat4&   GetProjectionMatrix     ()  const   { return projection; }

        [[nodiscard]] Mat4          GetViewProjectionMatrix ()  const   { return viewProjection; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetPosition     (const Vec3& pos);
        void    SetRotation     (const Vec3& rot);
        void    SetPerspective  (float aspectRatio, float fovRadians = DirectX::XM_PIDIV2, float nearPlane = 0.1f, float farPlane = 1000.0f);
        void    SetOrthographic (float left, float right, float up, float down, float nearPlane = 0.1f, float farPlane = 1000.0f);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void LookAt         (const Vec3& target);
        void MoveCamera     (const Vec3& movementVector);
        void RotateCamera   (const Vec3& rotationVector);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        float   fieldOfView;
        float   nearPlane;
        float   farPlane;

        Vec3    position;
        Vec3    rotation;

        Vec3    forward;
        Vec3    right;
        Vec3    back;
        Vec3    left;

        Mat4    view;
        Mat4    projection;
        Mat4    viewProjection;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void updateViewMatrix();

    };
}
