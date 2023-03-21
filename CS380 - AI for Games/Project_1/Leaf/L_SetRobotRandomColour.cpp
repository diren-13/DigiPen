/************************************************************************************//*!
\file           L_SetRobotRandomColour.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a SetRobotRandomColour Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_SetRobotRandomColour.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_SetRobotRandomColour::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_SetRobotRandomColour::on_update(float)
{
    const int   TARGET_ID   = agent->get_blackboard().get_value<int>("TargetID");
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

        const int TGT_ID = prospect->get_blackboard().get_value<int>("RobotID");
        if (TARGET_ID == TGT_ID)
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

    const Vec3& NEW_COLOUR = COLOUR_POOL[RNG::range<int>(0, static_cast<int>(std::size(COLOUR_POOL)))];
    target->set_color(NEW_COLOUR);

    on_success();

    display_leaf_text();
}

