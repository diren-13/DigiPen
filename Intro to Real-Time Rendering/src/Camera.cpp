/*+======================================================================================
File:       Camera.cpp

Date:       9th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for a Camera.

Functions:  RTR::Camera::Camera

            RTR::Camera::SetView
            RTR::Camera::LookAt
======================================================================================+*/

// Precompiled Header
#include "pch.h"
// External Dependencies
#include <gtc/constants.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/norm.hpp>
// Primary Header
#include "Camera.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Camera::Camera()
    : right             { 1.0f, 0.0f, 0.0f }
    , up                { 0.0, 1.0f, 0.0f }
    , forward           { 0.0f, 0.0f, 1.0f }
    , angles            { 0.0f, 0.0f, 0.0f }
    , position          { 0.0f, 0.0f, 0.0f }
    , aspectRatio       { 0.0f }
    , fov               { glm::pi<float>() * 0.25f }
    , nearPlane         { 0.1f }
    , farPlane          { 100.0f }
    , viewMatrix        { 1.0f }
    , perspectiveMatrix { 1.0f }
    {}


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void Camera::LookAt(const glm::vec3& target, float distanceFromTarget, const glm::vec3& orbitAngle)
    {
        glm::vec3 fromTarget = {0.0f, 0.0f, distanceFromTarget};
        glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };

        glm::vec3 angle = orbitAngle;

        static constexpr float ORBIT_THRESHOLD = glm::half_pi<float>();
        angle.x = std::clamp(angle.x, -ORBIT_THRESHOLD, ORBIT_THRESHOLD);

        // Rotate
        fromTarget  = glm::rotateX(fromTarget, angle.x);
        fromTarget  = glm::rotateY(fromTarget, angle.y);
        //fromTarget  += target;
        position    = target + fromTarget;

        worldUp     = glm::rotateZ(worldUp, angle.z);

        forward     = glm::normalize(target - position);
        right       = glm::normalize(glm::cross(forward, worldUp));
        up          = glm::normalize(glm::cross(forward, right));

        if (glm::length2(target - fromTarget) <= 0.001f || glm::abs(glm::length2(up) - 1.0f) > 0.001f)
            return;

        viewMatrix[0][0] = right.x;
        viewMatrix[1][0] = right.y;
        viewMatrix[2][0] = right.z;
        viewMatrix[3][0] = glm::dot(-right, position);

        viewMatrix[0][1] = up.x;
        viewMatrix[1][1] = up.y;
        viewMatrix[2][1] = up.z;
        viewMatrix[3][1] = glm::dot(-up, position);


        viewMatrix[0][2] = forward.x;
        viewMatrix[1][2] = forward.y;
        viewMatrix[2][2] = forward.z;
        viewMatrix[3][2] = glm::dot(-forward, position);

        viewMatrix[0][3] = viewMatrix[1][3] = viewMatrix[2][3] = 0;
        viewMatrix[3][3] = 1;
    }

    glm::mat4& Camera::ComputePespectiveMatrix(float fieldOfView, float ar, float nearValue, float farValue)
    {
        aspectRatio = ar;
        nearPlane   = nearValue;
        farPlane    = farValue;

        const float HALF_FOV = tan(fieldOfView * 0.5f);

        perspectiveMatrix = glm::mat4{ 1.0f };

        perspectiveMatrix[0][0] = 1.0f / (aspectRatio * HALF_FOV);
        perspectiveMatrix[1][1] = 1.0f / (HALF_FOV);
        perspectiveMatrix[2][2] = farPlane / (farPlane - nearPlane);
        perspectiveMatrix[2][3] = 1.0f;
        perspectiveMatrix[3][3] = 0.0f;
        perspectiveMatrix[3][2] = -(farPlane * nearPlane) / (farPlane - nearPlane);

        return perspectiveMatrix;
    }

} // namespace RTR