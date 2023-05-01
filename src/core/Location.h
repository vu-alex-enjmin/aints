#ifndef LOCATION_H_
#define LOCATION_H_

#include <cstdlib>

// Struct for representing 2D locations (Row + Column)
struct Location
{
    public:
        /*
        =========================================
            Attributes
        =========================================
        */

        // Row of location
        int Row;
        // Column of location
        int Col;

        
        /*
        =========================================
            Methods
        =========================================
        */

        // Create a location at Column 0 and Row 0
        Location();
        // Create a location at given row and column
        Location(int row, int col);

        // Equality operator
        bool operator==(const Location &other_r) const;
        // Difference operator
        bool operator!=(const Location &other_r) const;
        // Self-operator (used for hashing Location)
        size_t operator()(const Location &toHash_r) const noexcept;
};

#endif // LOCATION_H_
