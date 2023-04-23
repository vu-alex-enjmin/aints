#include "Task.h"

#include "TaskBoard.h"
#include "TaskAgent.h"

unsigned int Task::_nextId = 0;

Task::Task(int priority)
    : _id(_nextId++)
    , Priority(priority)
    , _board(nullptr)
    , _assignee(nullptr)
{

}

void Task::SetBoard(TaskBoard* board)
{
    _board = board;
}

void Task::AssignTo(TaskAgent* agent)
{
    _assignee = agent;
    agent->SetTask(this);
    _board->SetAssigned(this);
}

void Task::FreeUp()
{
    _assignee = nullptr;
    _board->SetUnassigned(this);
}

void Task::Cancel()
{
    if (_assignee != nullptr)
    {
        _assignee->SetTask(nullptr);
    }
    _board->RemoveAndFreeTask(this);
}

void Task::Complete()
{
    _board->RemoveAndFreeTask(this);
}

bool Task::IsAssigned() const
{
    return _assignee != nullptr;
}

int Task::GetId() const
{
    return _id;
}
