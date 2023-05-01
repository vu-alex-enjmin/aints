#ifndef WRAP_GRID_ALGORITHM_H_
#define WRAP_GRID_ALGORITHM_H_

#include <vector>
#include <queue>
#include <functional>

#include "Location.h"
#include "Ant.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // {N, E, S, W}

/*
    struct to store current state information
*/
class WrapGridAlgorithm
{
    public:
        static int Rows;
        static int Cols;

        // Initializes grid size used for all methods
        static void InitializeSize(const int rows, const int cols);
        // Returns the squared euclidean distance between two locations with the edges wrapped
        static int Distance2(const Location &a, const Location &b);
        // Returns the manhattan distance between two locations with the edges wrapped
        static int ManhattanDistance(const Location &a, const Location &b);
        // Returns the new location from moving in a given direction with the edges wrapped
        static Location GetLocation(const Location &startLoc, int direction);
        // If a path is found, pathDirections' content is replaced with the found path's directions
        // The path's directions are int values (0 for 'N', 1 for 'E', 2 for 'S', 3 for 'W')
        // in ascending order of the path (at index 0 is the first direction to follow from startLoc)
        // Returns whether a path was found
        static bool AStar
        (
            const Location &startLoc, 
            const Location &targetLoc, 
            std::vector<int> &pathDirections,
            std::function<bool(const Location&)> const &validLocationPredicate, // (currentLocation) -> isValid
            std::function<int(const Location&, const Location&)> const &heuristic = ManhattanDistance // (startLocation, currentLocation) -> heuristicValue
        );

        // Execute a BFS starting from a single start location, 
        // within 'range' (Manhattan Distance) of start location
        static void BreadthFirstSearchSingle
        (
            const Location &startLoc, 
            const int range,
            std::function<bool(const Location&)> const &validLocationPredicate, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
        );
        // Execute a BFS starting from several start locations, 
        // within 'range' (Manhattan Distance) of the start locations
        static void BreadthFirstSearchMultiple
        (
            const std::vector<Location> &startLocs, 
            const int range,
            std::function<bool(const Location&)> const &validLocationPredicate, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
        );
        // Execute a BFS starting from a single start location,
        // within a range of sqrt(radius2) (Euclidian Distance) of start location
        static void CircularBreadthFirstSearch
        (
            const Location &startLoc, 
            const int radius2,
            std::function<bool(const Location&)> const &validLocationPredicate, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited // (currentLocation, locationBfsDistance, directionTowardStart) -> interruptBfs
        );

    private:
        WrapGridAlgorithm() = default;

        // Execute a BFS using a base location queue and a base distances grid
        static void BreadthFirstSearch
        (
            std::queue<Location> &locQueue,
            std::vector<std::vector<int>> &distances,
            const int range,
            std::function<bool(const Location&)> const &validLocationPredicate, // (currentLocation) -> isValid
            std::function<bool(const Location&, const int, const int)> const &onVisited // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
        );
};

#endif // WRAP_GRID_ALGORITHM_H_
