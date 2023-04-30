#ifndef REACH_LOCATION_TASK_H_
#define REACH_LOCATION_TASK_H_

#include "AntTask.h"
#include "Location.h"

class ReachLocationTask : public AntTask
{
    public:
        ReachLocationTask(State *state, Location targetLocation, int stopDistance = 0);
        virtual void GiveOrderToAssignee() override;
        virtual bool IsValid() override;

    protected:
        Location _targetLocation;
        int _stopDistance;

        virtual int EvaluateCandidate(TaskAgent *candidate) override;
};

#endif // REACH_LOCATION_TASK_H_
