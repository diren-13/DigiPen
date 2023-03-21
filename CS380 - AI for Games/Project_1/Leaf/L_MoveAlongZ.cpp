/************************************************************************************//*!
\file           L_MoveAlongX.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a MoveAlongX Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_MoveAlongZ.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_MoveAlongZ::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_MoveAlongZ::on_update(float dt)
{
    const float MOVE_SPEED  = agent->get_movement_speed() * dt;
    const Vec3  CURRENT_POS = agent->get_position();

    Vec3 nextPos = CURRENT_POS;
    nextPos.z += MOVE_SPEED;
    agent->set_position(nextPos);

    on_success();

    display_leaf_text();
}

