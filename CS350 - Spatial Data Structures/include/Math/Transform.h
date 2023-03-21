/************************************************************************************//*!
\file           Transform.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a Transform.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "MathDefines.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Transform
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Transform ();
        Transform (const Vec3& pos, const Vec3& rot, const Vec3& scl);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] bool          GetDirty        ()  const   { return dirty; }
        [[nodiscard]] const Vec3&   GetPosition     ()  const   { return position; }
        [[nodiscard]] const Vec3&   GetRotation     ()  const   { return rotation; }
        [[nodiscard]] const Vec3&   GetScale        ()  const   { return scale; }
        [[nodiscard]] const Mat4&   GetTRS          ()  const   { return TRS; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetPosition (const Vec3& pos)                       { dirty = true; position = pos; }
        void    SetRotation (const Vec3& rot)                       { dirty = true; rotation = rot; }
        void    SetRotation (float pitch, float yaw, float roll)    { dirty = true; rotation = Vec3{pitch, yaw, roll}; }
        void    SetScale    (const Vec3& scl)                       { dirty = true; scale = scl; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void Update ();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        bool    dirty;
        Vec3    position;
        Vec3    rotation;
        Vec3    scale;
        Mat4    TRS;
    };
}