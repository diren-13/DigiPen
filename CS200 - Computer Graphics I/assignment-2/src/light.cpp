/************************************************************************************//*!
\file           light.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 18, 2021
\brief          This file contains the implementation of the class PointLight

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <light.h>

#include <glm/gtx/rotate_vector.hpp>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

PointLight::PointLight(const glm::dvec3& _intensity, const glm::dvec3& _pos)
: intensity		(_intensity)
, position		(_pos)
, rotation      (0.0)
, worldToModel	(0)
{}

/*---------------------------------------------------------------------------------*/
/* Member Functions                                                                */
/*---------------------------------------------------------------------------------*/

glm::vec3 PointLight::ComputeOutgoingLight(const glm::dvec3& pos, const glm::dvec3& normal, const glm::dvec3& reflectance)
{
    // Transform light position to model space
    glm::dvec3 lightPosModelSpace = worldToModel * glm::dvec4(position.x, position.y, position.z, 1.0);
    lightPosModelSpace = glm::rotateY(glm::dvec4(lightPosModelSpace, 1.0), rotation);

    // Compute normalised vectors
    glm::dvec3 vertexToLight			= glm::normalize(lightPosModelSpace - pos);	// lightSourcePos - objPos
    glm::dvec3 normalisedVertexNormal	= glm::normalize(normal);

    // Calculate incoming light
    glm::dvec3 incomingLight = std::max(0.0, glm::dot(normalisedVertexNormal, vertexToLight)) * intensity;

    return reflectance * incomingLight;
}