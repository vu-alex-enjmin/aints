#include "AntTask.h"

AntTask::AntTask(State *state)
    : Task()
    , _state(state)
{
    
}

AntTask::~AntTask()
{
    Unassign();
    _state->Bug << "AntTask Destroyed" << std::endl;
}