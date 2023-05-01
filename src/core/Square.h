#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

#include "Ant.h"

/*
    struct for representing a square in the Grid.
*/
struct Square
{
    public:
        /*
        =========================================
            Attributes
        =========================================
        */
        
        // Whether the Square is a known Water tile
        // This information never gets Reset 
        // Unknown tiles (in the Fog Of War) are not Water tiles until discovered
        bool IsWater;
        // Whether the Square is a visible Hill tile
        bool IsHill;
        // Whether the Square is a visible Food tile
        bool IsFood;
        // Hill's Player id
        // = -1 If the tile is not a Hill
        // = 0 If it's an Ally Hill
        // > 0 If it's an Enemy Hill
        int HillPlayer;
        // Turns spent in the Fog Of War
        // = 0 For visible tiles
        int TurnsInFog;
        // Info about the Ant that's on the Square
        // = nullptr when there is no Ant
        Ant* Ant;
        
        /*
        =========================================
            Methods
        =========================================
        */

        Square();
        // Resets the information for the square except water information
        void Reset();
};

#endif // SQUARE_H_
