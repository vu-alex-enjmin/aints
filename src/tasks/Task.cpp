#include "Task.h"

#include <limits>
#include "TaskAgent.h"

using namespace std;

unsigned int Task::_nextId = 0;

Task::Task()
    : _id(_nextId++)
    , _completed(false)
    , _assignee(nullptr)
    , _candidateFitnessPairs()
{
    _candidateFitnessPairs.reserve(4);
}

void Task::SetAsCompleted()
{
    _completed = true;
}

void Task::AddCandidate(TaskAgent *candidate)
{
    auto test = pair(candidate, EvaluateCandidate(candidate));
    _candidateFitnessPairs.push_back(test);
}

void Task::SelectCandidate()
{
    // Initialize selection
    TaskAgent *bestCandidate = nullptr;
    int bestCandidateFitness = numeric_limits<int>::min();

    // Search for candidate with best fitness
    for (const auto &candidateFitnessPair : _candidateFitnessPairs)
    {
        // Skip candidates which already have a task assigned
        if (candidateFitnessPair.first->HasTask())
            continue;

        if (candidateFitnessPair.second > bestCandidateFitness)
        {
            bestCandidate = candidateFitnessPair.first;
            bestCandidateFitness = candidateFitnessPair.second;
        }
    }
    
    if (bestCandidate == nullptr)
        return;

    // If a best candidate was found, assign task to best candidate
    bestCandidate->CurrentTask = this;
    _assignee = bestCandidate;
}

void Task::ClearCandidates()
{
    _candidateFitnessPairs.clear();
}

void Task::Unassign()
{
    if (_assignee == nullptr)
        return;
    
    _assignee->CurrentTask = nullptr;
    _assignee = nullptr;
}

bool Task::IsAssigned() const
{
    return _assignee != nullptr;
}

bool Task::IsCompleted() const
{
    return _completed;
}

int Task::GetId() const
{
    return _id;
}

Task::~Task()
{
    Unassign();
}