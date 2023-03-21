#include <pch.h>
#include "D_InvertedRepeater.h"

void D_InvertedRepeater::on_update(float dt)
{
    BehaviorNode *child = children.front();

    child->tick(dt);

    if (child->succeeded() == true)
    {
        on_failure();
    }
    else if (child->failed() == true)
    {
        on_success();
    }

}

void D_InvertedRepeater::on_exit()
{
    // we want the node and child to repeat, so go back to ready
    set_status(NodeStatus::READY);
}

