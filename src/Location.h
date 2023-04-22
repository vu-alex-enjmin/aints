#ifndef LOCATION_H_
#define LOCATION_H_

#include <cstdlib>

/*
    struct for representing locations in the Grid.
*/
struct Location
{
    int Row, Col;

    Location();

    Location(int row, int col);

    bool operator==(const Location &other) const;
    bool operator!=(const Location &other) const;

    size_t operator()(const Location &toHash) const noexcept;
};

#endif // LOCATION_H_
