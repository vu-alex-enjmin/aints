#include "ReachLocationTask.h"

ReachLocationTask::ReachLocationTask(State *state, Location targetLocation, int stopDistance)
    : AntTask(state)
    , _targetLocation(targetLocation)
    , _stopDistance(stopDistance)
{

}

void ReachLocationTask::GiveOrderToAssignee()
{
    Ant *assignee = (Ant*) _assignee;

    std::vector<int> path = _state->AStar(assignee->CurrentLocation, _targetLocation);
    if (path.size() > _stopDistance)
    {
        int firstDirection = path[0];
        assignee->SetMoveDirection(firstDirection);
    }
    else
    {
        assignee->SetMoveDirection();
        _completed = true;
    }
}

bool ReachLocationTask::IsValid()
{
    return !_state->Grid[_targetLocation.Row][_targetLocation.Col].IsWater;
}

int ReachLocationTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    return _state->Rows + _state->Cols - _state->ManhattanDistance(antCandidate->CurrentLocation, _targetLocation);
}