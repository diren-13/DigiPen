/************************************************************************************//*!
\file           D_IfTargetInForwardRange.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 23, 2022
\brief          Contains the interface for a IfTargetInForwardRange Decorator object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "BehaviorNode.h"

/*-------------------------------------------------------------------------------------*/
/* Type Definitions                                                                    */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
@brief  Requires a "ForwardRange" integer variable on the agent's Blackboard.
*//*************************************************************************************/
class D_IfRobotInForwardRange : public BaseNode<D_IfRobotInForwardRange>
{
protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    virtual void on_update  (float dt)  override;
};

