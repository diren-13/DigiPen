/************************************************************************************//*!
\file           D_TargetSizeHit.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 18, 2022
\brief          Contains the interface for a TargetSizeHit Decorator node object.
 
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

class D_IfTargetSizeHit : public BaseNode<D_IfTargetSizeHit>
{
protected:
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    void on_update  (float dt)  override;
};





