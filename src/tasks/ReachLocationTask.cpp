#include "ReachLocationTask.h"

#include "WrapGridAlgorithm.h"

ReachLocationTask::ReachLocationTask(State *state, Location targetLocation, int stopDistance)
    : AntTask(state)
    , _targetLocation(targetLocation)
    , _stopDistance(stopDistance)
{

}

void ReachLocationTask::GiveOrderToAssignee()
{
    Ant *assignee = (Ant*) _assignee;

    std::vector<int> pathDirections;
    if (WrapGridAlgorithm::AStar(
        assignee->CurrentLocation, 
        _targetLocation, 
        pathDirections, 
        [&](const Location &loc) { return !_state->Grid[loc.Row][loc.Col].IsWater; }
    ))
    {
        if (pathDirections.size() > _stopDistance)
        {
            int firstDirection = pathDirections[0];
            assignee->SetMoveDirection(firstDirection);
        }
        else
        {
            assignee->SetMoveDirection();
            _completed = true;
        }
    }
}

bool ReachLocationTask::IsValid()
{
    return !_state->Grid[_targetLocation.Row][_targetLocation.Col].IsWater;
}

int ReachLocationTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    return _state->Rows + _state->Cols - WrapGridAlgorithm::ManhattanDistance(antCandidate->CurrentLocation, _targetLocation);
}