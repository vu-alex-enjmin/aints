#ifndef ANT_H
#define ANT_H
#include "Location.h"
struct Ant
{
    Location CurrentLocation;
    // current turn's definitive direction (no overwrite possible once decision is taken)
    bool Decided;
    int MoveDirection;
    Location NextLocation;

    Ant(Location location);
};

#endif // ANT_H