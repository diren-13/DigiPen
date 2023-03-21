#pragma once
#include "BehaviorNode.h"

class D_RepeatFourTimes : public BaseNode<D_RepeatFourTimes>
{
public:
    D_RepeatFourTimes();

protected:
    unsigned counter;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};