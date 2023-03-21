#pragma once
#include "BehaviorNode.h"

class C_Selector : public BaseNode<C_Selector>
{
public:
    C_Selector();

protected:
    size_t currentIndex;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};