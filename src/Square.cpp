#include "Square.h"

Square::Square()
    : IsVisible(false)
    , IsWater(false)
    , IsHill(false)
    , IsFood(false)
    , Ant(-1)
    , HillPlayer(-1)
{

}

void Square::Reset()
{
    IsVisible = false;
    IsHill = false;
    IsFood = false;
    Ant = -1;
    HillPlayer = -1;
}