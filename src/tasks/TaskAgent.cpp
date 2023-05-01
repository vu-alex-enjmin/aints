#include "TaskAgent.h"

TaskAgent::TaskAgent()
    : CurrentTask_p(nullptr)
{

}

// Unassigns CurrentTask_p on destruction
TaskAgent::~TaskAgent()
{
    if (CurrentTask_p == nullptr)
        return;
    
    CurrentTask_p->Unassign();
}

// Checks if CurrentTask_p points to something
bool TaskAgent::HasTask() const
{
    return CurrentTask_p != nullptr;
}