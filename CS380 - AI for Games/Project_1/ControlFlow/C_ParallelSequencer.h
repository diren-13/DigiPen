#pragma once
#include "BehaviorNode.h"

class C_ParallelSequencer : public BaseNode<C_ParallelSequencer>
{
protected:
    virtual void on_update(float dt) override;
};