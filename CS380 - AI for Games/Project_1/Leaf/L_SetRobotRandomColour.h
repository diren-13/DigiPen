/************************************************************************************//*!
\file           L_SetRobotRandomColour.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the interface for a SetRobotRandomColour Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "BehaviorNode.h"

/*-------------------------------------------------------------------------------------*/
/* Type Definitions                                                                    */
/*-------------------------------------------------------------------------------------*/

class L_SetRobotRandomColour : public BaseNode<L_SetRobotRandomColour>
{
protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    void on_enter   ()          override;
    void on_update  (float dt)  override;

private:
    /*---------------------------------------------------------------------------------*/
    /* Static Data Members                                                             */
    /*---------------------------------------------------------------------------------*/
    static constexpr Vec3 COLOUR_POOL[9] =
    {
        Vec3 { 1.0f, 0.5f, 0.0f }
    ,   Vec3 { 0.0f, 1.0f, 0.0f }
    ,   Vec3 { 0.0f, 1.0f, 0.5f }
    ,   Vec3 { 0.5f, 1.0f, 0.0f }
    ,   Vec3 { 0.0f, 1.0f, 1.0f }
    ,   Vec3 { 0.0f, 0.0f, 1.0f }
    ,   Vec3 { 0.5f, 0.0f, 1.0f }
    ,   Vec3 { 0.0f, 0.5f, 1.0f }
    ,   Vec3 { 1.0f, 0.0f, 1.0f }
    };
};
