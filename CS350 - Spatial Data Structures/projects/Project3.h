/************************************************************************************//*!
\file           Project3.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of the interface for Project3.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
#include <map>
// Project Headers
#include "ClamChowder.h"

/*-------------------------------------------------------------------------------------*/
/* Type Definitions                                                                    */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
@brief    Encapsulates the Rendering Application.
*//*************************************************************************************/
class Project3
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------------*/
    /* LifeCycle Functions                                                             */
    /*---------------------------------------------------------------------------------*/
    void Initialise ();
    void Update     ();
    void Exit       ();

private:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    CC::Window*             window          = nullptr;
    CC::Engine*             engine          = nullptr;
    CC::Editor*             editor          = nullptr;

    // Scene specific
    int                     selectedIndex   = 0;
    float                   lightRotation   = CC::Math::Radians(45.0f);

    std::vector<CC::Scene>  scenes;
    std::vector<CC::View>   views;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    void setupResources     ();
    void setupScenes        ();
    void drawEditor         ();
    void moveMainCamera     (CC::Camera& cam);
    void moveLight          (CC::Light& light);
};

