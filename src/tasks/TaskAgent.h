#ifndef TASK_AGENT_H_
#define TASK_AGENT_H_

#include "Task.h"

class TaskAgent
{
    public:
        TaskAgent();
        void SetTask(Task* task);
        void FreeTask();
        void CompleteTask();

    private:
        Task* _currentTask;
};

#endif // TASK_AGENT_H_
