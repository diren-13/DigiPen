/************************************************************************************//*!
\file           C_If.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a If Control Flow object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "C_IfElse.h"

void C_IfElse::on_enter()
{
    BehaviorNode::on_enter();
}


void C_IfElse::on_update(float dt)
{
    // Get condition
    BehaviorNode* child = children.front();
    child->tick(dt);

    bool condition = false;

    if (child->succeeded())
    {
        condition = true;
    }
    else if (child->failed())
    {
        condition = false;
    }

    // Get next child
    child = condition ? children[1] : children[2];
    child->tick(dt);

    set_status(child->get_status());
    set_result(child->get_result());
}
