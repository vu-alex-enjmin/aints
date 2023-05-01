#include "ReachLocationTask.h"

#include "WrapGridAlgorithm.h"

ReachLocationTask::ReachLocationTask(State *state_p, Location targetLocation, int stopDistance)
    : AntTask(state_p)
    , _targetLocation(targetLocation)
    , _stopDistance(stopDistance)
{

}

// Leads the assignee (Ant) towards its target Location
void ReachLocationTask::GiveOrderToAssignee()
{
    Ant *assignee_p = (Ant*) _assignee_p;

    std::vector<int> pathDirections;
    if (WrapGridAlgorithm::AStar(
        assignee_p->CurrentLocation, 
        _targetLocation, 
        pathDirections, 
        [&](const Location &loc_r) { return !_state_p->Grid[loc_r.Row][loc_r.Col].IsWater; }
    ))
    {
        if (pathDirections.size() > _stopDistance)
        {
            // Make assignee move in the direction of the first A* step towards _targetLocation
            int firstDirection = pathDirections[0];
            assignee_p->SetMoveDirection(firstDirection);
        }
        else // when the path is shorter than the stopping distance, the task is completed
        {
            assignee_p->SetMoveDirection();
            _completed = true;
        }
    }
}

// returns true when the target is not a Water tile
bool ReachLocationTask::IsValid()
{
    return !_state_p->Grid[_targetLocation.Row][_targetLocation.Col].IsWater;
}

int ReachLocationTask::EvaluateCandidate(TaskAgent *candidate_p) 
{
    Ant *antCandidate_p = (Ant*) candidate_p;
    // Evaluate candidate based on its distance from its targetLocation
    return _state_p->Rows + _state_p->Cols - WrapGridAlgorithm::ManhattanDistance(antCandidate_p->CurrentLocation, _targetLocation);
}