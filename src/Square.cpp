#include "Square.h"

Square::Square()
{
    IsVisible = IsWater = IsHill = IsFood = 0;
    Ant = HillPlayer = -1;
}

void Square::Reset()
{
    IsVisible = 0;
    IsHill = 0;
    IsFood = 0;
    Ant = HillPlayer = -1;
}