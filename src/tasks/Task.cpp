#include "Task.h"

#include <limits>
#include "TaskAgent.h"

using namespace std;

unsigned int Task::_nextId = 0;

Task::Task()
    : _id(_nextId++)
    , _completed(false)
    , _assignee_p(nullptr)
    , _candidateFitnessPairs()
{
    // Allocate memory for at least 4 candidates
    _candidateFitnessPairs.reserve(4);
}

// Sets the Task as performed and finished successfully
void Task::SetAsCompleted()
{
    _completed = true;
}

// Adds candidate to vector of candidates after calculating its fitness
void Task::AddCandidate(TaskAgent *candidate_p)
{
    auto test = pair(candidate_p, EvaluateCandidate(candidate_p));
    _candidateFitnessPairs.push_back(test);
}

// Finds fittest candidate among current candidates and puts it as assignee
// (Note that the fittest candidate has the highest fitness value)
void Task::SelectCandidate()
{
    // Initialize selection
    TaskAgent *bestCandidate_p = nullptr;
    int bestCandidateFitness = numeric_limits<int>::min();

    // Search for candidate with best fitness
    for (const auto &candidateFitnessPair_r : _candidateFitnessPairs)
    {
        // Skip candidates which already have a task assigned
        if (candidateFitnessPair_r.first->HasTask())
            continue;

        if (candidateFitnessPair_r.second > bestCandidateFitness)
        {
            bestCandidate_p = candidateFitnessPair_r.first;
            bestCandidateFitness = candidateFitnessPair_r.second;
        }
    }
    
    if (bestCandidate_p == nullptr)
        return;

    // If a best candidate was found, assign task to best candidate
    bestCandidate_p->CurrentTask_p = this;
    _assignee_p = bestCandidate_p;
}

// Removes all candidates from vector of candidates
void Task::ClearCandidates()
{
    _candidateFitnessPairs.clear();
}

// Removes task from assignee and remove assignee 
void Task::Unassign()
{
    if (_assignee_p == nullptr)
        return;
    
    _assignee_p->CurrentTask_p = nullptr;
    _assignee_p = nullptr;
}

// Checks if the Task has an assignee
bool Task::IsAssigned() const
{
    return _assignee_p != nullptr;
}

// Checks if Task was previously SetAsCompleted
bool Task::IsCompleted() const
{
    return _completed;
}

// Returns the Task's unique id
int Task::GetId() const
{
    return _id;
}

// Unassigns the Task on destruction
Task::~Task()
{
    Unassign();
}