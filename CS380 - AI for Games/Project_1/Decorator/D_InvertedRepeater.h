#pragma once
#include "BehaviorNode.h"

class D_InvertedRepeater : public BaseNode<D_InvertedRepeater>
{
protected:
    virtual void on_update(float dt) override;
    virtual void on_exit() override;
};