#include <pch.h>
#include "D_RepeatFourTimes.h"

D_RepeatFourTimes::D_RepeatFourTimes() : counter(0)
{}

void D_RepeatFourTimes::on_enter()
{
    counter = 0;
    BehaviorNode::on_enter();
}

void D_RepeatFourTimes::on_update(float dt)
{
    // repeat the child until 4 successes, stop on any failure
    BehaviorNode *child = children.front();

    child->tick(dt);

    if (child->succeeded() == true)
    {
        ++counter;

        if (counter == 4)
        {
            on_success();
        }
        else
        {
            child->set_status(NodeStatus::READY);
        }
    }
    else if (child->failed() == true)
    {
        on_failure();
    }
}
