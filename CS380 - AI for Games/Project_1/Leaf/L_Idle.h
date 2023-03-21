#pragma once
#include "BehaviorNode.h"

class L_Idle : public BaseNode<L_Idle>
{
public:
    L_Idle();

protected:
    float timer;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};