#ifndef ANT_H_
#define ANT_H_

#include "Location.h"

struct Ant
{
    int Team;
    int Id;
    Location CurrentLocation;
    // current turn's definitive direction (no overwrite possible once decision is taken)
    bool Decided;
    int MoveDirection;
    Location NextLocation;
    // Combat/Defense properties
    int SurroundingOpponentCount;

    Ant(int team, Location location);
};

#endif // ANT_H_