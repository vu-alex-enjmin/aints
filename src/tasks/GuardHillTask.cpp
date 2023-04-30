#include "GuardHillTask.h"

#include <limits>
#include <vector>

#include "TaskAgent.h"

using namespace std;

GuardHillTask::GuardHillTask(State *state, Location guardedLocation)
    : AntTask(state)
    , _guardedLocation(guardedLocation)
{

}

void GuardHillTask::GiveOrderToAssignee()
{
    Ant *assignee = (Ant*) _assignee;

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
    return _state->MyHills.size() > 0;
}

int GuardHillTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    return _state->Rows + _state->Cols - _state->ManhattanDistance(antCandidate->CurrentLocation, _guardedLocation);
}
