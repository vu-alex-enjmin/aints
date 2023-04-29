#include "TaskAgent.h"

TaskAgent::TaskAgent()
    : CurrentTask(nullptr)
{

}

bool TaskAgent::HasTask() const
{
    return CurrentTask != nullptr;
}