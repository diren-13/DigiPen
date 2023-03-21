/************************************************************************************//*!
\file           L_SpawnRobot.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the interface for a SpawnRobot Leaf node object.
 
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

/************************************************************************************//*!
@brief  Behaviour Node to Spawn a new agent.

        A "Spawn Limit" Blackboard value is required to limit the number of spawns. If
        this value is <= 0, spawning will never stop.
*//*************************************************************************************/
class L_SpawnRobot : public BaseNode<L_SpawnRobot>
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Constructor                                                                     */
    /*---------------------------------------------------------------------------------*/
    L_SpawnRobot();

protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    void on_enter   ()          override;
    void on_update  (float dt)  override;

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    int spawnCount;
};
