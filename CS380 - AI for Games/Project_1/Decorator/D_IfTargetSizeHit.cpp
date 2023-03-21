/************************************************************************************//*!
\file           D_TargetSizeHit.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a TargetSizeHit Decorator node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "D_IfTargetSizeHit.h"
// Project Headers
#include "BehaviorTreeBuilder.h"
#include "../AgentStuff/StateTrackers.h"

void D_IfTargetSizeHit::on_update(float dt)
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
    
    const Vec3& TARGET_SIZE = target->get_scaling();
    const Vec3& MAX_SIZE    = agent->get_scaling();

    if (TARGET_SIZE.x < MAX_SIZE.x && TARGET_SIZE.y < MAX_SIZE.y && TARGET_SIZE.z < MAX_SIZE.z)
    {
        on_failure();
    }
    else
    {
        BehaviorNode* child = children.front();
        child->tick(dt);

        set_status(child->get_status());
        set_result(child->get_result());
    }

    display_leaf_text();
}

