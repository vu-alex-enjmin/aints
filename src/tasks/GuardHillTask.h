#ifndef GUARD_HILL_TASK_H_
#define GUARD_HILL_TASK_H_

#include "AntTask.h"
#include "Location.h"
#include "Square.h"

class GuardHillTask : public AntTask
{
    public:
        GuardHillTask(State *state, Location guardedLocation);
        virtual void GiveOrderToAssignee() override;
        virtual bool IsValid() override;

    protected:
        Location _guardedLocation;

        virtual int EvaluateCandidate(TaskAgent *candidate) override;
};

#endif // GUARD_HILL_TASK_H_
