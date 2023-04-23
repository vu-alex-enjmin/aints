#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

#include "Ant.h"

/*
    struct for representing a square in the Grid.
*/
struct Square
{
    bool IsWater, IsHill, IsFood;
    int HillPlayer, TurnsInFog;
    Ant* Ant;

    Square();
    // Resets the information for the square except water information
    void Reset();
};

#endif // SQUARE_H_