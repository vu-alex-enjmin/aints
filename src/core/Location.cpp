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
bool Location::operator==(const Location &other) const 
{
    return (this->Col == other.Col) && (this->Row == other.Row);
}

// Difference operator
bool Location::operator!=(const Location &other) const 
{
    return (this->Col != other.Col) || (this->Row != other.Row);
}

// Self-operator (used for hashing Location)
size_t Location::operator()(const Location &toHash) const noexcept
{
    // https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
    return (toHash.Row + toHash.Col) * (toHash.Row + toHash.Col + 1) / 2 + toHash.Row;
}