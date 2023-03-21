/************************************************************************************//*!
\file           L_ClosestRobotInRange.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a ClosestRobotInRange Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Headers
#include "L_FindClosestRobot.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_FindClosestRobot::on_enter()
{
    BehaviorNode::on_leaf_enter();
}

void L_FindClosestRobot::on_update(float)
{
    // Reset Target ID every frame
    agent->get_blackboard().set_value<int>("TargetID", -1);

    float           smallestDist    = std::numeric_limits<float>().max();
    BehaviorAgent*  target          = nullptr;

    // Look through all agents
    // Skip Spawners and Builders
    for (const auto& a : agents->get_all_agents())
    {
        BehaviorAgent* prospect = reinterpret_cast<BehaviorAgent*>(a);

        const auto& ROLE = prospect->get_blackboard().get_value<std::string>("Role");
        if (ROLE != "Robot")
            continue;

        // Check if robot is in front of current agent
        const float DIST = std::abs(Vec3::Distance(prospect->get_position(), agent->get_position()));

        if (DIST < smallestDist)
        {
            smallestDist        = DIST;
            target              = prospect;
            const int TARGET_ID = prospect->get_blackboard().get_value<int>("RobotID");

            agent->get_blackboard().set_value<int>("TargetID", TARGET_ID);
        }
    }

    target ? on_success() : on_failure();
}
