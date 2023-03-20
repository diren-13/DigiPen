/*+======================================================================================
File:       Camera.h

Date:       9th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Camera.

Classes:    RTR::Camera
======================================================================================+*/

#pragma once

// External Headers
#include <glm.hpp>

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a generic Camera.
    /// </summary>
    class Camera
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Camera();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const glm::vec3&       GetRight()              const   { return right; }
        [[nodiscard]] inline const glm::vec3&       GetUp()                 const   { return up; }
        [[nodiscard]] inline const glm::vec3&       GetForward()            const   { return forward; }
        [[nodiscard]] inline const glm::vec3&       GetAngles()             const   { return angles; }
        [[nodiscard]] inline const glm::vec3&       GetPosition()           const   { return position; }

        [[nodiscard]] inline float                  GetAspectRatio()        const   { return aspectRatio; }
        [[nodiscard]] inline float                  GetFOV()                const   { return fov; }
        [[nodiscard]] inline float                  GetNearPlane()          const   { return nearPlane; }
        [[nodiscard]] inline float                  GetFarPlane()           const   { return farPlane; }

        [[nodiscard]] inline const glm::mat4&       GetViewMatrix()         const   { return viewMatrix; }
        [[nodiscard]] inline const glm::mat4&       GetPerspectiveMatrix()  const   { return perspectiveMatrix; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline glm::vec3&             Right()                         { return right; }
        [[nodiscard]] inline glm::vec3&             Up()                            { return up; }
        [[nodiscard]] inline glm::vec3&             Forward()                       { return forward; }
        [[nodiscard]] inline glm::vec3&             Angles()                        { return angles; }
        [[nodiscard]] inline glm::vec3&             Position()                      { return position; }

        [[nodiscard]] inline float&                 AspectRatio()                   { return aspectRatio; }
        [[nodiscard]] inline float&                 FOV()                           { return fov; }
        [[nodiscard]] inline float&                 NearPlane()                     { return nearPlane; }
        [[nodiscard]] inline float&                 FarPlane()                      { return farPlane; }

        [[nodiscard]] inline glm::mat4&             ViewMatrix()                    { return viewMatrix; }
        [[nodiscard]] inline glm::mat4&             PerspectiveMatrix()             { return perspectiveMatrix; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void        LookAt                  (const glm::vec3& target, float distanceFromTarget, const glm::vec3& orbitAngle);
        glm::mat4&  ComputePespectiveMatrix (float fieldOfView, float ar, float nearValue = 0.1f, float farValue = 100.0f);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        glm::vec3   right;
        glm::vec3   up;
        glm::vec3   forward;

        // X = Pitch, Y = Yaw, Z = Roll
        glm::vec3   angles;

        glm::vec3   position;

        float       aspectRatio;
        float       fov;

        float       nearPlane;
        float       farPlane;

        glm::mat4   viewMatrix;
        glm::mat4   perspectiveMatrix;
    };
} // namespace RTR