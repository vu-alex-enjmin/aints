#ifndef TASK_BOARD_H_
#define TASK_BOARD_H_

#include <unordered_map>

#include "Task.h"

class TaskBoard
{
    public:
        std::unordered_map<unsigned int, Task*> AvailableTasks;
        std::unordered_map<unsigned int, Task*> AssignedTasks;

        void AddTask(Task* task);
        void SetAssigned(Task* task);
        void SetUnassigned(Task* task);
        void RemoveAndFreeTask(Task* task);
};

#endif // TASK_BOARD_H
