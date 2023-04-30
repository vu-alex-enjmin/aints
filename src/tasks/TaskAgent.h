#ifndef TASK_AGENT_H_
#define TASK_AGENT_H_

#include "Task.h"

class TaskAgent
{
    public:
        Task *CurrentTask;
        TaskAgent();
        ~TaskAgent();
        bool HasTask() const;
};

#endif // TASK_AGENT_H_
