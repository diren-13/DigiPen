/************************************************************************************//*!
\file           D_IfTargetInForwardRange.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a IfTargetInForwardRange Decorator object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "D_IfRobotInForwardRange.h"

void D_IfRobotInForwardRange::on_update(float dt)
{
    const Vec3& POS             = agent->get_position();
    const float OFFSET          = agent->get_scaling().x * 0.5f;
    const float LEFT_Z_OFFSET   = POS.z - OFFSET;
    const float RIGHT_Z_OFFSET  = POS.z + OFFSET;

    const float FORWARD_RANGE   = agent->get_blackboard().get_value<float>("ForwardRange");

    // Get Target
    const int TARGET_ID = agent->get_blackboard().get_value<int>("TargetID");

    BehaviorAgent* target = nullptr;
    // Search list of agents for matching target
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

    // Check if in forward range
    const Vec3& TARGET_POS  = target->get_position();
    const float DIST        = std::abs(Vec3::Distance(TARGET_POS, agent->get_position()));
    if (TARGET_POS.z < LEFT_Z_OFFSET || TARGET_POS.z > RIGHT_Z_OFFSET || DIST > FORWARD_RANGE)
    {
        on_failure();
        return;
    }

    BehaviorNode* child = children.front();
    child->tick(dt);

    set_status(child->get_status());
    set_result(child->get_result());
}

