#include "ReachAntTask.h"

#include "WrapGridAlgorithm.h"

ReachAntTask::ReachAntTask(State *state, int targetAntId, int stopDistance)
    : AntTask(state)
    , _targetAntId(targetAntId)
    , _stopDistance(stopDistance)
{

}

void ReachAntTask::GiveOrderToAssignee()
{
    Ant *assignee = (Ant*) _assignee;
    Ant *targetAnt = _state->AllyAnts[_targetAntId];
    
    std::vector<int> pathDirections;
    if (WrapGridAlgorithm::AStar(
        assignee->CurrentLocation, 
        targetAnt->CurrentLocation, 
        pathDirections, 
        [&](const Location &loc) { return !_state->Grid[loc.Row][loc.Col].IsWater; }
    ))
    {
        if (pathDirections.size() > (_stopDistance + 1))
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

bool ReachAntTask::IsValid()
{
    return (_state->AllyAnts.count(_targetAntId) > 0);
}

int ReachAntTask::EvaluateCandidate(TaskAgent *candidate) 
{
    Ant *antCandidate = (Ant*) candidate;
    Ant *targetAnt = _state->AllyAnts[_targetAntId];

    return _state->Rows + _state->Cols - WrapGridAlgorithm::ManhattanDistance(antCandidate->CurrentLocation, targetAnt->CurrentLocation);
}
