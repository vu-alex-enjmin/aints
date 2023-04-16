#ifndef ANT_H
#define ANT_H
#include "Location.h"
struct Ant
{
    int Team;
    Location CurrentLocation;
    // current turn's definitive direction (no overwrite possible once decision is taken)
    bool Decided;
    int MoveDirection;
    Location NextLocation;

    Ant(int team, Location location);
};

#endif // ANT_H