#ifndef REACH_ANT_TASK_H_
#define REACH_ANT_TASK_H_

#include "Ant.h"
#include "AntTask.h"

class ReachAntTask : public AntTask
{
    public:
        ReachAntTask(State *state, int targetAntId, int stopDistance = 0);
        virtual void GiveOrderToAssignee() override;
        virtual bool IsValid() override;

    protected:
        int _targetAntId;
        int _stopDistance;

        virtual int EvaluateCandidate(TaskAgent *candidate) override;
};

#endif // REACH_ANT_TASK_H_
