/************************************************************************************//*!
\file           L_IncreaseTargetSize.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a IncreaseTargetSize Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_IncreaseRobotSize.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_IncreaseRobotSize::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_IncreaseRobotSize::on_update(float dt)
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
    
    const Vec3& START_SIZE  { 0.5f, 0.5f, 0.5f };
    const Vec3& MAX_SIZE    = agent->get_scaling();
    const Vec3& DELTA       = (MAX_SIZE - START_SIZE) * dt;
    Vec3 newSize            = target->get_scaling() + DELTA;
    newSize.Clamp(START_SIZE, MAX_SIZE, newSize);
    target->set_scaling(newSize);

    on_success();

    display_leaf_text();
}


