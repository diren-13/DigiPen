/************************************************************************************//*!
\file           L_IsRobotBuilding.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a IsRobotBuilding Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_IsRobotBuilding.h"
// Project Headers
#include "BehaviorTreeBuilder.h"
#include "../AgentStuff/StateTrackers.h"

void L_IsRobotBuilding::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_IsRobotBuilding::on_update(float dt)
{
    const auto&         BLACKBOARD      = agent->get_blackboard();
    const RobotState    BUILD_STATE     = BLACKBOARD.get_value<RobotState>("Building");
    const RobotState    PROGRAM_STATE   = BLACKBOARD.get_value<RobotState>("Programming");

    const bool IS_BUILDING      = (BUILD_STATE == RobotState::IN_PROGRESS);
    const bool IS_PROGRAMMING   = (PROGRAM_STATE == RobotState::IN_PROGRESS);
    (IS_BUILDING || IS_PROGRAMMING) ? on_success() : on_failure();

    display_leaf_text();
}


