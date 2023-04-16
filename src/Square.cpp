#include "Square.h"

Square::Square()
    : TurnsInFog(1)
    , IsWater(false)
    , IsHill(false)
    , IsFood(false)
    , Ant(-1, Location(-1,-1))
    , HillPlayer(-1)
{

}

void Square::Reset()
{
    TurnsInFog++;
    IsHill = false;
    IsFood = false;
    Ant.Decided = false;
    Ant.NextLocation = Location(-1, -1);
    Ant.Team = -1;
    HillPlayer = -1;
}