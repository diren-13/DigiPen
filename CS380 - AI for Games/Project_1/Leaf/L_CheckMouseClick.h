#pragma once
#include "BehaviorNode.h"

class L_CheckMouseClick : public BaseNode<L_CheckMouseClick>
{
protected:
    virtual void on_update(float dt);
};