/************************************************************************************//*!
\file           D_IfSpawnerEnabled.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a IfSpawnerEnabled Decorator node object.

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "D_IfSpawnerEnabled.h"
// Project Headers
#include "BehaviorTreeBuilder.h"


void D_IfSpawnerEnabled::on_update(float dt)
{
    const bool SPAWN_STATE  = agent->get_blackboard().get_value<bool>("SpawnState");

    if (SPAWN_STATE == true)
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

