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

#pragma once

// Project Headers
#include "BehaviorNode.h"

/*-------------------------------------------------------------------------------------*/
/* Type Definitions                                                                    */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
@brief  Encapsulates the behaviour of a numeric switch case. The intended use is for
        selecting subtrees.

        Requires a "SwitchTree" int variable on the blackboard.
*//*************************************************************************************/
class C_Switch : public BaseNode<C_Switch>
{
protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    virtual void on_enter   ()          override;
    virtual void on_update  (float dt)  override;
};
