#include "TaskAgent.h"

TaskAgent::TaskAgent()
    : CurrentTask(nullptr)
{

}

TaskAgent::~TaskAgent()
{
    if (CurrentTask == nullptr)
        return;
    
    CurrentTask->Unassign();
}

bool TaskAgent::HasTask() const
{
    return CurrentTask != nullptr;
}