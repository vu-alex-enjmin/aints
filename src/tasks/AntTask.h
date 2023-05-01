#ifndef ANT_TASK_H_
#define ANT_TASK_H_

#include "Task.h"
#include "State.h"

class AntTask : public Task
{
    public:
        /*
        =========================================
            Methods
        =========================================
        */

        AntTask(State *state);

        virtual void GiveOrderToAssignee() override = 0;
        virtual bool IsValid() override = 0;

    protected:
        
        /*
        =========================================
            Attributes
        =========================================
        */

        // Pointer to the Game's State instance
        State *_state;

        
        /*
        =========================================
            Methods
        =========================================
        */

        virtual int EvaluateCandidate(TaskAgent *candidate) override = 0;
};

#endif // ANT_TASK_H_