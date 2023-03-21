/************************************************************************************//*!
\file           L_SpawnRobot.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a SpawnRobot Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_SpawnRobot.h"
// Project Headers
#include "BehaviorTreeBuilder.h"
#include "../AgentStuff/StateTrackers.h"

L_SpawnRobot::L_SpawnRobot()
: spawnCount { 0 }
{}

void L_SpawnRobot::on_enter()
{
    const auto& BLACKBOARD  = agent->get_blackboard();
    const int   SPAWN_LIMIT = BLACKBOARD.get_value<int>("SpawnLimit");

    if (spawnCount < SPAWN_LIMIT || SPAWN_LIMIT <= 0)
    {
        BehaviorNode::on_leaf_enter();
    }
    else
    {
        on_failure();
    }
}

void L_SpawnRobot::on_update(float)
{
    const std::string NEW_ROBOT_NAME = "Robot " + std::to_string(spawnCount);
    BehaviorAgent* newRobot = agents->create_behavior_agent(NEW_ROBOT_NAME.c_str(), BehaviorTreeTypes::Robot);

    Vec3 newAgentPos = agent->get_position();
    newAgentPos.x += 5.0f;
    newRobot->set_position(newAgentPos);
    newRobot->set_scaling(0.5f);
    newRobot->set_yaw(0.0f);

    auto& newRobotBlackboard = newRobot->get_blackboard();
    newRobotBlackboard.set_value<std::string>("Role", "Robot");
    newRobotBlackboard.set_value<int>("RobotID", spawnCount);
    newRobotBlackboard.set_value<int>("SubTreeCount", 2);
    newRobotBlackboard.set_value<int>("SubTreeIndex", 0);
    newRobotBlackboard.set_value<RobotState>("Building", RobotState::NOT_DONE);
    newRobotBlackboard.set_value<RobotState>("Programming", RobotState::NOT_DONE);


    ++spawnCount;
    auto& blackboard = agent->get_blackboard();
    blackboard.set_value<int>("SpawnCount", spawnCount);

    on_success();

    display_leaf_text();
}

