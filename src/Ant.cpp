#include "Ant.h"
Ant::Ant(int team, Location location)
    : Team(team)
    , CurrentLocation(location)
    , Decided(false)
    , MoveDirection(-1)
    , NextLocation(-1, -1)
{
    
}