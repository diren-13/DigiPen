/************************************************************************************//*!
\file           L_PrevSubTree.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the implementation for a PrevSubTree Leaf node object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "L_PrevSubTree.h"
// Project Headers
#include "BehaviorTreeBuilder.h"

void L_PrevSubTree::on_enter()
{
    BehaviorNode::on_leaf_enter();
}


void L_PrevSubTree::on_update(float dt)
{
    auto&   blackboard = agent->get_blackboard();
    int     subTreeIdx = blackboard.get_value<int>("SubTreeIndex");

    --subTreeIdx;
    blackboard.set_value<int>("SubTreeIndex", subTreeIdx);

    on_success();

    display_leaf_text();
}

