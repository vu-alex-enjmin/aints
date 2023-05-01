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

// Makes the ant cecide to Move in a direction
// Ignored when already Decided
void Ant::SetMoveDirection(int moveDirection)
{
    if (Decided)
        return;
    
    MoveDirection = moveDirection;
    Decided = true;
}

// Remove the ant's current Move direction and makes it undecided
void Ant::ResetMoveDirection()
{
    MoveDirection = -1;
    Decided = false;
}