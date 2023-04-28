#ifndef ANT_H_
#define ANT_H_

#include "Location.h"

struct Ant
{
    public:
        int Team;
        int Id;
        Location CurrentLocation;
        // Current turn's definitive direction (no overwrite possible once decision is taken)
        bool Decided;
        int MoveDirection;
        // Next turn's actual location (either location after move (if successful), or current location)
        Location NextLocation;
        // Combat/Defense properties
        int SurroundingOpponentCount;

        Ant(int team, Location location);

        void SetMoveDirection(int moveDirection = -1);
        void ResetMoveDirection();
};

#endif // ANT_H_