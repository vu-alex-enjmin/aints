#ifndef WRAP_GRID_ALGORITHM_H_
#define WRAP_GRID_ALGORITHM_H_

#include <vector>
#include <queue>
#include <functional>

#include "Location.h"
#include "Ant.h"

// Constants
//  Total number of move directions
const int TDIRECTIONS = 4;
//  Coordinate offsets for each move direction
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // {N, E, S, W}

// Class for all algorithms related to pathfinding, distance computation & finding locations
// inside a grid which wraps at the edges in the two axii (Row & Column)
class WrapGridAlgorithm
{
    public:
        
        /*
        =========================================
            Attributes
        =========================================
        */

        // Number of rows of the grid (used for almost all computation)
        static int Rows;
        // Number of columns of the grid (used for almost all computation)
        static int Cols;

        
        /*
        =========================================
            Methods
        =========================================
        */

        // Initializes grid size used for all methods
        static void InitializeSize(const int rows, const int cols);
        // Returns the squared euclidean distance between two locations with the edges wrapped
        static int Distance2(const Location &loc1_r, const Location &loc2_r);
        // Returns the manhattan distance between two locations with the edges wrapped
        static int ManhattanDistance(const Location &loc1_r, const Location &loc2_r);
        // Returns the new location from moving in a given direction with the edges wrapped
        static Location GetLocation(const Location &startLoc_r, int direction);
        // Executes the A* algorithm in order to find a path to 'targetLoc_r' starting from 'startLoc_r'.
        // If a path is found, pathDirections_r' content is replaced with the found path's directions
        // The path's directions are int values (0 for 'N', 1 for 'E', 2 for 'S', 3 for 'W')
        // in ascending order of the path (at index 0 is the first direction to follow from startLoc_r)
        // Returns whether a path was found.
        static bool AStar
        (
            const Location &startLoc_r, 
            const Location &targetLoc_r, 
            std::vector<int> &pathDirections_r,
            std::function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
            std::function<int(const Location&, const Location&)> const &heuristic_f = ManhattanDistance // (startLocation, currentLocation) -> heuristicValue
        );
        // Executes a BFS starting from a single start location, 
        // within 'range' (Manhattan Distance) of the start location
        static void BreadthFirstSearchSingle
        (
            const Location &startLoc_r, 
            const int range,
            std::function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
        );
        // Executes a BFS starting from several start locations, 
        // within 'range' (Manhattan Distance) of the start locations
        static void BreadthFirstSearchMultiple
        (
            const std::vector<Location> &startLocs, 
            const int range,
            std::function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
        );
        // Executes a BFS starting from a single start location,
        // within a range of sqrt(radius2) (Euclidian Distance) of the start location
        static void CircularBreadthFirstSearch
        (
            const Location &startLoc_r, 
            const int radius2,
            std::function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationBfsDistance, directionTowardStart) -> interruptBfs
        );

    private:

        /*
        =========================================
            Methods
        =========================================
        */
        // Prevents creation of a WrapGridAlgorithm instance
        WrapGridAlgorithm() = default;

        // Executes a BFS using a base location queue and a base distances grid
        static void BreadthFirstSearch
        (
            std::queue<Location> &locQueue,
            std::vector<std::vector<int>> &distances_r,
            const int range,
            std::function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
        );
};

#endif // WRAP_GRID_ALGORITHM_H_
