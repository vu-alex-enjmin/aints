#include "GuardHillTask.h"

#include <limits>
#include <vector>

#include "WrapGridAlgorithm.h"

using namespace std;

GuardHillTask::GuardHillTask(State *state_p, Location guardedLocation)
    : AntTask(state_p)
    , _guardedLocation(guardedLocation)
{

}

// Leads the assignee (Ant) towards its _guardedLocation
void GuardHillTask::GiveOrderToAssignee()
{
    Ant *assignee_p = (Ant*) _assignee_p;

    std::vector<int> pathDirections;
    if (WrapGridAlgorithm::AStar(
        assignee_p->CurrentLocation, 
        _guardedLocation, 
        pathDirections, 
        [&](const Location &loc_r) { return !_state_p->Grid[loc_r.Row][loc_r.Col].IsWater; }
    ))
    {
        if(pathDirections.size() > 0)
        {
            // Make assignee move in the direction of the first A* step towards _guardedLocation
            int firstDirection = pathDirections[0];
            assignee_p->SetMoveDirection(firstDirection);
        }
        else
        {
            // Don't move if the ant is already at its destination
            assignee_p->SetMoveDirection();
        }
    }
}

// Returns true if there are still hills to guard
bool GuardHillTask::IsValid()
{
    return _state_p->MyHills.size() > 0;
}

int GuardHillTask::EvaluateCandidate(TaskAgent *candidate_p) 
{
    Ant *antCandidate_p = (Ant*) candidate_p;
    // Evaluate candidate based on its distance from its guardedLocation
    return _state_p->Rows + _state_p->Cols - WrapGridAlgorithm::ManhattanDistance(antCandidate_p->CurrentLocation, _guardedLocation);
}
