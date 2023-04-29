#ifndef GUARD_HILL_TASK_H_
#define GUARD_HILL_TASK_H_

#include <unordered_map>

#include "AntTask.h"
#include "Location.h"
#include "Square.h"

class GuardHillTask : public AntTask
{
    public:
        GuardHillTask(State *state, Location guardedLocation, Square *hillSquare);
        virtual void GiveOrderToAssignee() override;
        virtual bool IsValid() override;

    protected:
        Square *_hillSquare;
        Location _guardedLocation;

        virtual int EvaluateCandidate(TaskAgent *candidate) override;
};

#endif // GUARD_HILL_TASK_H_
