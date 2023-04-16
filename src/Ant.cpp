#include "Ant.h"
Ant::Ant(Location location)
    : CurrentLocation(location)
    , Decided(false)
    , MoveDirection(-1)
    , NextLocation(-1, -1)
{
    
}