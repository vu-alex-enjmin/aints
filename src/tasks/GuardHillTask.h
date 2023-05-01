#ifndef GUARD_HILL_TASK_H_
#define GUARD_HILL_TASK_H_

#include "AntTask.h"
#include "Location.h"
#include "Square.h"

class GuardHillTask : public AntTask
{
    public:
        /*
        =========================================
            Methods
        =========================================
        */

        GuardHillTask(State *state, Location guardedLocation);

        virtual void GiveOrderToAssignee() override;
        virtual bool IsValid() override;

    protected:
        /*
        =========================================
            Attributes
        =========================================
        */

        // Location of the hill that is being guarded
        Location _guardedLocation;

        /*
        =========================================
            Methods
        =========================================
        */

        virtual int EvaluateCandidate(TaskAgent *candidate) override;
};

#endif // GUARD_HILL_TASK_H_
