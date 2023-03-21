#pragma once
#include "BehaviorNode.h"

class C_Sequencer : public BaseNode<C_Sequencer>
{
public:
    C_Sequencer();
protected:
    size_t currentIndex;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
    
};