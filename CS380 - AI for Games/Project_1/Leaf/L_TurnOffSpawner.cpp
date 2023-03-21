/************************************************************************************//*!
\file           L_ChangeSpawnerState.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a TurnOffSpawner Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_TurnOffSpawner.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_TurnOffSpawner::on_update(float)
{
    agent->get_blackboard().set_value<bool>("SpawnState", false);
    on_success();
}
