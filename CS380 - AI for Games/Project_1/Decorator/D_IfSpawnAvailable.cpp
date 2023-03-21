/************************************************************************************//*!
\file           L_IfSpawnAvailable.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a IfSpawnAvailable Decorator node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "D_IfSpawnAvailable.h"
// Project Headers
#include "BehaviorTreeBuilder.h"


void D_IfSpawnAvailable::on_update(float dt)
{
    const auto& BLACKBOARD  = agent->get_blackboard();
    const int   SPAWN_COUNT = BLACKBOARD.get_value<int>("SpawnCount");
    const int   SPAWN_LIMIT = BLACKBOARD.get_value<int>("SpawnLimit");

    if (SPAWN_LIMIT > 0 && SPAWN_COUNT < SPAWN_LIMIT)
    {
        BehaviorNode* child = children.front();
        child->tick(dt);

        set_status(child->get_status());
        set_result(child->get_result());
    }
    else
    {
        on_failure();
    }

    display_leaf_text();
}

