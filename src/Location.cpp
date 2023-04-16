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

bool Location::operator==(const Location &other) const 
{
    return (this->Col == other.Col) && (this->Row == other.Row);
}

bool Location::operator!=(const Location &other) const 
{
    return (this->Col != other.Col) || (this->Row != other.Row);
}

size_t Location::operator()(const Location &toHash) const noexcept
{
    return (toHash.Row + toHash.Col) * (toHash.Row + toHash.Col + 1) / 2 + toHash.Row;
}