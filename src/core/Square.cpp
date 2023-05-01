#include "Square.h"

Square::Square()
    : TurnsInFog(1)
    , IsWater(false)
    , IsHill(false)
    , IsFood(false)
    , Ant_p(nullptr)
    , HillPlayer(-1)
{

}

void Square::Reset()
{
    TurnsInFog++;
    IsHill = false;
    IsFood = false;
    Ant_p = nullptr;
    HillPlayer = -1;
}