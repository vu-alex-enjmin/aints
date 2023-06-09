#ifndef TASK_AGENT_H_
#define TASK_AGENT_H_

#include "Task.h"

// Performer of Tasks that it is given
class TaskAgent
{
    public:
        /*
        =========================================
            Attributes
        =========================================
        */
       
        // Currently assigned Task
        Task *CurrentTask_p;

        /*
        =========================================
            Methods
        =========================================
        */

        TaskAgent();
        // Unassigns CurrentTask on destruction
        ~TaskAgent();
        // Checks if CurrentTask points to something
        bool HasTask() const;
};

#endif // TASK_AGENT_H_
