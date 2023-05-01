#ifndef ANT_H_
#define ANT_H_

#include "TaskAgent.h"
#include "Location.h"

class Ant : public TaskAgent
{
    public:
        
        /*
        =========================================
            Attributes
        =========================================
        */

        // Team id
        // = 0 for Allies
        // > 0 for Enemies
        int Team;
        // Unique identifier
        int Id;
        // Current location on the Grid
        Location CurrentLocation;

        // > (Currently only used by Ally Ants)

        // Whether the Ant's MoveDirection has been set
        bool Decided;
        // Direction to move in at the end of the turn
        int MoveDirection;
        // Next turn's actual location (either location after move (if successful), or current location)
        Location NextLocation;
        
        /*
        =========================================
            Methods
        =========================================
        */

        Ant(int team, Location location);

        // Makes the ant cecide to Move in a direction
        // Ignored when already Decided
        void SetMoveDirection(int moveDirection = -1);
        // Remove the ant's current Move direction and makes it undecided
        void ResetMoveDirection();
};

#endif // ANT_H_