/************************************************************************************//*!
\file           L_PrevSubTree.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the interface for a PrevSubTree Leaf node object.
 
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
@brief  Behaviour Node to move to the previous sub tree.

        Requires a "SubTreeIndex" Blackboard integer variable.
*//*************************************************************************************/
class L_PrevSubTree : public BaseNode<L_PrevSubTree>
{
protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    void on_enter   ()          override;
    void on_update  (float dt)  override;
};




