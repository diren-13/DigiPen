#include <pch.h>
#include "C_ParallelSequencer.h"

void C_ParallelSequencer::on_update(float dt)
{
    // run all children
    // if any child fails, the node fails
    // if all children succeed, the node succeeds

    bool allSuccess = true;

    for (auto && child : children)
    {
        child->tick(dt);

        if (child->failed() == true)
        {
            on_failure();
        }
        else if (child->is_running() == true && child->succeeded() == false)
        {
            allSuccess = false;
        }
    }

    if (allSuccess == true)
    {
        on_success();
    }
}
