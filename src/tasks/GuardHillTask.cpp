#include "GuardHillTask.h"

#include <limits>
#include "TaskAgent.h"

using namespace std;

GuardHillTask::GuardHillTask(State *state, Location guardedLocation, Square *hillSquare)
    : AntTask(state)
    , _guardedLocation(guardedLocation)
    , _hillSquare(hillSquare)
{

}

void GuardHillTask::GiveOrderToAssignee()
{
    Ant *assignee = (Ant*) _assignee;
    
    // Calcul du next move/direction et du decided
    
    // Astar

    // Prendre la premiere direction indiquée par le A*
    std::vector<int> path = _state->AStar(assignee->CurrentLocation, _guardedLocation);
    if(path.size() > 0)
    {
        int firstDirection = path[0];
        assignee->SetMoveDirection(firstDirection);
    }
    else
    {
        assignee->SetMoveDirection();
    }
}

bool GuardHillTask::IsValid()
{
    return true;
}

int GuardHillTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    return _state->Rows + _state->Cols - _state->ManhattanDistance(antCandidate->CurrentLocation, _guardedLocation);
}
