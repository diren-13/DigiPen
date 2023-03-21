/************************************************************************************//*!
\file           C_Switch.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a If Condition Decorator object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "C_Switch.h"

void C_Switch::on_enter()
{
    BehaviorNode::on_enter();
}

void C_Switch::on_update(float dt)
{
    const auto& BLACKBOARD  = agent->get_blackboard();
    const int   SUBTREE     = BLACKBOARD.get_value<int>("SubTreeIndex");

    BehaviorNode* child = children[SUBTREE];
    child->tick(dt);

    set_status(child->get_status());
    set_result(child->get_result());
}

