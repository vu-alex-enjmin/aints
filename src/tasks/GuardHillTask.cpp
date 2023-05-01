#include "GuardHillTask.h"

#include <limits>
#include <vector>

#include "WrapGridAlgorithm.h"

using namespace std;

GuardHillTask::GuardHillTask(State *state, Location guardedLocation)
    : AntTask(state)
    , _guardedLocation(guardedLocation)
{

}

// Leads the assignee (Ant) towards its _guardedLocation
void GuardHillTask::GiveOrderToAssignee()
{
    Ant *assignee = (Ant*) _assignee;

    std::vector<int> pathDirections;
    if (WrapGridAlgorithm::AStar(
        assignee->CurrentLocation, 
        _guardedLocation, 
        pathDirections, 
        [&](const Location &loc) { return !_state->Grid[loc.Row][loc.Col].IsWater; }
    ))
    {
        if(pathDirections.size() > 0)
        {
            // Make assignee move in the direction of the first A* step towards _guardedLocation
            int firstDirection = pathDirections[0];
            assignee->SetMoveDirection(firstDirection);
        }
        else
        {
            // Don't move if the ant is already at its destination
            assignee->SetMoveDirection();
        }
    }
}

// Returns true if there are still hills to guard
bool GuardHillTask::IsValid()
{
    return _state->MyHills.size() > 0;
}

int GuardHillTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    // Evaluate candidate based on its distance from its guardedLocation
    return _state->Rows + _state->Cols - WrapGridAlgorithm::ManhattanDistance(antCandidate->CurrentLocation, _guardedLocation);
}
