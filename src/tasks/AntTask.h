#ifndef ANT_TASK_H_
#define ANT_TASK_H_

#include "Task.h"
#include "State.h"

class AntTask : public Task
{
    public:
        AntTask(State *state);
        ~AntTask();

        virtual void GiveOrderToAssignee() override = 0;
        virtual bool IsValid() override = 0;

    protected:
        State *_state;

        virtual int EvaluateCandidate(TaskAgent *candidate) override = 0;
};

#endif // ANT_TASK_H_