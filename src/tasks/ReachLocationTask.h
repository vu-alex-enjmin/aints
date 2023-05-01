#ifndef REACH_LOCATION_TASK_H_
#define REACH_LOCATION_TASK_H_

#include "AntTask.h"
#include "Location.h"

// Tasks an Ant to move towards a Location
class ReachLocationTask : public AntTask
{
    public:
        
        /*
        =========================================
            Methods
        =========================================
        */

        ReachLocationTask(State *state, Location targetLocation, int stopDistance = 0);
        // Leads the assignee (Ant) towards its target Location
        virtual void GiveOrderToAssignee() override;
        // returns true when the target is not a Water tile
        virtual bool IsValid() override;

    protected:
        
        /*
        =========================================
            Attributes
        =========================================
        */

        // Location to reach
        Location _targetLocation;
        // Distance from the target (in steps to take) at which the location is considered reached
        int _stopDistance;

        /*
        =========================================
            Methods
        =========================================
        */

        virtual int EvaluateCandidate(TaskAgent *candidate) override;
};

#endif // REACH_LOCATION_TASK_H_
