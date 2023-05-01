#include "Location.h"

// Create a location at Column 0 and Row 0
Location::Location()
    : Row(0)
    , Col(0)
{

}

// Create a location at given row and column
Location::Location(int row, int col)
    : Row(row)
    , Col(col)
{

}

// Equality operator
bool Location::operator==(const Location &other_r) const 
{
    return (this->Col == other_r.Col) && (this->Row == other_r.Row);
}

// Difference operator
bool Location::operator!=(const Location &other_r) const 
{
    return (this->Col != other_r.Col) || (this->Row != other_r.Row);
}

// Self-operator (used for hashing Location)
size_t Location::operator()(const Location &toHash_r) const noexcept
{
    // https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
    return (toHash_r.Row + toHash_r.Col) * (toHash_r.Row + toHash_r.Col + 1) / 2 + toHash_r.Row;
}