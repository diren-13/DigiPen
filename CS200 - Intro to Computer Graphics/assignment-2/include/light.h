/************************************************************************************//*!
\file           light.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 18, 2021
\brief          This file contains the a class that encapulates the behaviour of a light
                source at a point.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <utility.h>      // Colour

/********************************************************************************//*!
@brief    Encapsulates a light source at a point. Calculations are made in the 
          coordinate system of the object being illuminated.
*//*********************************************************************************/
struct PointLight
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    glm::dvec3  intensity;		// Do not store alpha for now
    glm::dvec3  position;
    double      rotation;
    glm::mat4   worldToModel;

    /*---------------------------------------------------------------------------------*/
    /* Constructors																	   */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief      Default Constructor for PointLight.

    @param[in]  _intensity
        The intensity of the light as a colour in normalised values. Default to white.
    @param[in]  _pos
        The position of the light source. Default at origin.
    *//*****************************************************************************/
    PointLight(const glm::dvec3& _intensity = { 1.0, 1.0, 1.0 }, const glm::dvec3& _pos = { 0.0, 0.0, 0.0 });

    /*---------------------------------------------------------------------------------*/
    /* Member Functions																   */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief      Computes the outgoing light applied to a surface based on this
                light source. Position and normal supplied must be in the model's
                coordinate system.

    @param[in]  pos
        The position FROM the light source.
    @param[in]  normal
        A vertex or surface normal.
    @param[in]  reflectance
        Normalised colour values of the percentage of colours that the surface will 
        reflect.

    @returns    A normalised vector towards this light source.
    *//*****************************************************************************/
    glm::vec3 ComputeOutgoingLight(const glm::dvec3& pos, const glm::dvec3& normal, const glm::dvec3& reflectance = { 1.0, 1.0, 1.0 });

  private:

};
