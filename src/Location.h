#ifndef LOCATION_H_
#define LOCATION_H_

/*
    struct for representing locations in the Grid.
*/
struct Location
{
    int Row, Col;

    Location();

    Location(int row, int col);
};

#endif //LOCATION_H_
