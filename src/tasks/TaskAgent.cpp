#include "TaskAgent.h"

TaskAgent::TaskAgent()
    : CurrentTask(nullptr)
{

}

// Unassigns CurrentTask on destruction
TaskAgent::~TaskAgent()
{
    if (CurrentTask == nullptr)
        return;
    
    CurrentTask->Unassign();
}

// Checks if CurrentTask points to something
bool TaskAgent::HasTask() const
{
    return CurrentTask != nullptr;
}