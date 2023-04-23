#include "TaskBoard.h"

void TaskBoard::AddTask(Task* task)
{
    AvailableTasks[task->GetId()] = task;
    task->SetBoard(this);
}

void TaskBoard::SetAssigned(Task* task)
{
    AssignedTasks[task->GetId()] = task;
    AvailableTasks.erase(task->GetId());
}

void TaskBoard::SetUnassigned(Task* task)
{
    AvailableTasks[task->GetId()] = task;
    AssignedTasks.erase(task->GetId());
}

void TaskBoard::RemoveAndFreeTask(Task* task)
{
    AvailableTasks.erase(task->GetId());
    AssignedTasks.erase(task->GetId());
    delete task;
}
