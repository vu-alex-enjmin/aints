#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
    struct for representing a square in the Grid.
*/
struct Square
{
    bool IsVisible, IsWater, IsHill, IsFood;
    int Ant, HillPlayer;

    Square();
    //resets the information for the square except water information
    void Reset();
};

#endif //SQUARE_H_
