#pragma once
#include "BehaviorNode.h"

class L_MoveToFurthestAgent : public BaseNode<L_MoveToFurthestAgent>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

private:
    Vec3 targetPoint;
};