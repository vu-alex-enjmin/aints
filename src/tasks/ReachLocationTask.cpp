#include "ReachLocationTask.h"

#include "WrapGridAlgorithm.h"

ReachLocationTask::ReachLocationTask(State *state, Location targetLocation, int stopDistance)
    : AntTask(state)
    , _targetLocation(targetLocation)
    , _stopDistance(stopDistance)
{

}

// Leads the assignee (Ant) towards its target Location
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
            // Make assignee move in the direction of the first A* step towards _targetLocation
            int firstDirection = pathDirections[0];
            assignee->SetMoveDirection(firstDirection);
        }
        else // when the path is shorter than the stopping distance, the task is completed
        {
            assignee->SetMoveDirection();
            _completed = true;
        }
    }
}

// returns true when the target is not a Water tile
bool ReachLocationTask::IsValid()
{
    return !_state->Grid[_targetLocation.Row][_targetLocation.Col].IsWater;
}

int ReachLocationTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    // Evaluate candidate based on its distance from its targetLocation
    return _state->Rows + _state->Cols - WrapGridAlgorithm::ManhattanDistance(antCandidate->CurrentLocation, _targetLocation);
}