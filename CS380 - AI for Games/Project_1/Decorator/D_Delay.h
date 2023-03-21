#pragma once
#include "BehaviorNode.h"

class D_Delay : public BaseNode<D_Delay>
{
public:
    D_Delay();

protected:
    float delay;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};