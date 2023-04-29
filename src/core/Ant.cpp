#include "Ant.h"

Ant::Ant(int team, Location location)
    : TaskAgent() 
    , Team(team)
    , CurrentLocation(location)
    , Decided(false)
    , MoveDirection(-1)
    , NextLocation(-1, -1)
{
    static int currentId = 0;
    Id = currentId;
    currentId ++;
}

void Ant::SetMoveDirection(int moveDirection)
{
    MoveDirection = moveDirection;
    Decided = true;
}

void Ant::ResetMoveDirection()
{
    MoveDirection = -1;
    Decided = false;
}