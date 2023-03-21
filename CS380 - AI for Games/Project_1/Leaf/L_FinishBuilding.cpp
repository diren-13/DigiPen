/************************************************************************************//*!
\file           L_FinishBuilding.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a FinishBuilding Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_FinishBuilding.h"
// Project Headers
#include "BehaviorTreeBuilder.h"
#include "../AgentStuff/StateTrackers.h"

void L_FinishBuilding::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_FinishBuilding::on_update(float)
{
    const int TARGET_ID = agent->get_blackboard().get_value<int>("TargetID");
    if (TARGET_ID < 0)
    {
        on_failure();
        return;
    }

    BehaviorAgent* target = nullptr;
    for (auto& a : agents->get_all_agents())
    {
        BehaviorAgent* prospect = reinterpret_cast<BehaviorAgent*>(a);

        const auto& ROLE = prospect->get_blackboard().get_value<std::string>("Role");
        if (ROLE != "Robot")
            continue;

        const int PROSPECT_ID = prospect->get_blackboard().get_value<int>("RobotID");
        if (PROSPECT_ID == TARGET_ID)
        {
            target = prospect;
            break;
        }
    }

    if (!target)
    {
        on_failure();
        return;
    }

    // Check if robot can be built
    RobotState buildState = target->get_blackboard().get_value<RobotState>("Building");
    if (buildState == RobotState::IN_PROGRESS)
    {
        buildState = RobotState::DONE;
        target->get_blackboard().set_value<RobotState>("Building", buildState);

        on_success();
    }
    else
    {
        on_failure();
    }
}
