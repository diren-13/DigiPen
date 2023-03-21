#include <pch.h>
#include "C_Selector.h"

C_Selector::C_Selector() : currentIndex(0)
{}

void C_Selector::on_enter()
{
    currentIndex = 0;
    BehaviorNode::on_enter();
}

void C_Selector::on_update(float dt)
{
    // if any child succeeds, node succeeds
    // if all children fail, node fails
    BehaviorNode *currentNode = children[currentIndex];
    currentNode->tick(dt);

    if (currentNode->succeeded() == true)
    {
        on_success();
    }
    else if (currentNode->failed() == true)
    {
        // move to the next node
        ++currentIndex;

        // if we hit size, that means all nodes failed
        if (currentIndex == children.size())
        {
            on_failure();
        }
    }
}
