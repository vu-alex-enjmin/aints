#include "TaskAgent.h"

TaskAgent::TaskAgent()
    : _currentTask(nullptr)
{

}

void TaskAgent::SetTask(Task* task)
{
    _currentTask = task;
}

void TaskAgent::FreeTask()
{
    if (_currentTask == nullptr)
        return;
    
    _currentTask->FreeUp();
    _currentTask = nullptr;
}

void TaskAgent::CompleteTask()
{
    _currentTask->Complete();
    _currentTask = nullptr;
}
