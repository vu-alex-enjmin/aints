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
       
        GuardHillTask(State *state_p, Location guardedLocation);

        // Leads the assignee (Ant) towards its _guardedLocation
        virtual void GiveOrderToAssignee() override;
        // Returns true if there are still hills to guard
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

        virtual int EvaluateCandidate(TaskAgent *candidate_p) override;
};

#endif // GUARD_HILL_TASK_H_
