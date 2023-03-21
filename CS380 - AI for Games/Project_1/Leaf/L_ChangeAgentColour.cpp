/************************************************************************************//*!
\file           L_ChangeAgentColour.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a ChangeAgentColour Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_ChangeAgentColour.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_ChangeAgentColour::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_ChangeAgentColour::on_update(float)
{
    const Vec3& NEW_COLOUR = agent->get_blackboard().get_value<Vec3>("TargetColour");
    agent->set_color(NEW_COLOUR);

    on_success();
}
