#include "Location.h"

Location::Location()
    : Row(0)
    , Col(0)
{

}

Location::Location(int row, int col)
    : Row(row)
    , Col(col)
{

}

bool Location::operator==(const Location &other) const {
    return this->Col == other.Col && this->Row == other.Row;
}