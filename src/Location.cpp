#include "Location.h"

Location::Location()
{
    Row = Col = 0;
}

Location::Location(int row, int col)
{
    Row = row;
    Col = col;
}