#include "WrapGridAlgorithm.h"

#include <algorithm>
#include <cmath>
#include <queue>

using namespace std;

// Number of rows of the grid (used for almost all computation)
int WrapGridAlgorithm::Rows = 0;
// Number of columns of the grid (used for almost all computation)
int WrapGridAlgorithm::Cols = 0;

// Initializes grid size used for all methods
void WrapGridAlgorithm::InitializeSize(const int rows, const int cols)
{
    Rows = rows;
    Cols = cols;
}

// Returns the squared euclidean distance between two locations with the edges wrapped
int WrapGridAlgorithm::Distance2(const Location &loc1_r, const Location &loc2_r)
{
    int d1 = abs(loc1_r.Row-loc2_r.Row),
        d2 = abs(loc1_r.Col-loc2_r.Col),
        dr = min(d1, Rows-d1),
        dc = min(d2, Cols-d2);
    return dr*dr + dc*dc;
}

// Returns the manhattan distance between two locations with the edges wrapped
int WrapGridAlgorithm::ManhattanDistance(const Location &loc1_r, const Location &loc2_r)
{
    int rowDist = abs(loc1_r.Row-loc2_r.Row);
    int colDist = abs(loc1_r.Col-loc2_r.Col);
    // Evaluating distance both ways (since the map loops)
    int bestRowDist = min(rowDist, Rows-rowDist);
    int bestColDist = min(colDist, Cols-colDist);
    
    return bestRowDist + bestColDist;
}

// Returns the new location from moving in a given direction with the edges wrapped
Location WrapGridAlgorithm::GetLocation(const Location &loc_r, int direction)
{
    return Location( (loc_r.Row + DIRECTIONS[direction][0] + Rows) % Rows,
                     (loc_r.Col + DIRECTIONS[direction][1] + Cols) % Cols );
}


// If a path is found, pathDirections' content is replaced with the found path's directions
// The path's directions are int values (0 for 'N', 1 for 'E', 2 for 'S', 3 for 'W')
// in ascending order of the path (at index 0 is the first direction to follow from startLoc)
// Returns whether a path was found.
bool WrapGridAlgorithm::AStar
(
    const Location &startLoc_r, 
    const Location &targetLoc_r, 
    vector<int> &pathDirections_r,
    function<bool(const Location&)> const &validLocationPredicate_f, // (newLocation) -> isValid
    function<int(const Location&, const Location&)> const &heuristic_f // (startLocation, currentLocation) -> heuristicValue
)
{
    // Initialization of collections
    //  actual distance to target
    vector<vector<int>> distances(Rows, vector<int>(Cols, numeric_limits<int>::max()));
    //  score (distance to target + heuristic)
    vector<vector<int>> scores(Rows, vector<int>(Cols, numeric_limits<int>::max()));
    //  directions taken to reach a given location (used to reconstruct the path)
    vector<vector<int>> directions(Rows, vector<int>(Cols, -1));
    auto comparator_f = [&](const Location a, const Location b)
    {
        return scores[a.Row][a.Col] > scores[b.Row][b.Col];
    };
    priority_queue<Location, vector<Location>, decltype(comparator_f)> locQueue(comparator_f);

    // Prime the algorithm
    locQueue.push(startLoc_r);
    scores[startLoc_r.Row][startLoc_r.Col] = 0;
    distances[startLoc_r.Row][startLoc_r.Col] = 0;

    // Search for path
    Location currLoc, nextLoc;
    int currLocDist, nextLocDist;
    while (!locQueue.empty())
    {
        currLoc = locQueue.top();
        // Compute path that was taken to reach target and return it
        if (currLoc == targetLoc_r)
        {
            pathDirections_r.clear();
            
            while (currLoc != startLoc_r)
            {
                pathDirections_r.push_back(directions[currLoc.Row][currLoc.Col]);
                currLoc = GetLocation(currLoc, (directions[currLoc.Row][currLoc.Col] + TDIRECTIONS / 2) % TDIRECTIONS);
            }
            reverse(pathDirections_r.begin(), pathDirections_r.end());

            return true;
        }
        
        // Add/Update unvisited neighbours
        locQueue.pop();
        currLocDist = distances[currLoc.Row][currLoc.Col];
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            nextLoc = GetLocation(currLoc, d);
            // If next location is invalid, skip it
            if (!validLocationPredicate_f(nextLoc))
                continue;

            nextLocDist = currLocDist + 1;
            if (nextLocDist < distances[nextLoc.Row][nextLoc.Col])
            {
                directions[nextLoc.Row][nextLoc.Col] = d;
                distances[nextLoc.Row][nextLoc.Col] = nextLocDist;
                scores[nextLoc.Row][nextLoc.Col] = nextLocDist + heuristic_f(startLoc_r, nextLoc);
                locQueue.push(nextLoc);
            }
        }
    }

    return false;
}

// Executes a BFS starting from a single start location, 
// within 'range' (Manhattan Distance) of the start location
void WrapGridAlgorithm::BreadthFirstSearchSingle
(
    const Location &startLoc_r, 
    const int range,
    function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
    function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
)
{
    // Initialize algorithm
    queue<Location> locQueue;
    vector<vector<int>> distances(Rows, vector<int>(Cols, -1));

    // Prime BFS' values for start location
    if (onVisited_f(startLoc_r, 0, -1)) // Stop BFS on the start location if needed
    {
        return;
    }
    locQueue.push(startLoc_r);
    distances[startLoc_r.Row][startLoc_r.Col] = 0;

    // Executes main BFS Algorithm
    BreadthFirstSearch(locQueue, distances, range, validLocationPredicate_f, onVisited_f);
}

// Executes a BFS starting from several start locations, 
// within 'range' (Manhattan Distance) of the start locations
void WrapGridAlgorithm::BreadthFirstSearchMultiple
(
    const vector<Location> &startLocs_r, 
    const int range,
    function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
    function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
)
{
    // Initialize algorithm
    queue<Location> locQueue;
    vector<vector<int>> distances(Rows, vector<int>(Cols, -1));

    // Prime BFS' values for each start location
    for(auto &startLoc_r : startLocs_r)
    {
        if (onVisited_f(startLoc_r, 0, -1)) // Stop BFS on a start location if needed
        {
            return;
        }
        locQueue.push(startLoc_r);
        distances[startLoc_r.Row][startLoc_r.Col] = 0;
    }

    // Executes main BFS Algorithm
    BreadthFirstSearch(locQueue, distances, range, validLocationPredicate_f, onVisited_f);
}

// Executes a BFS starting from a single start location,
// within a range of sqrt(radius2) (Euclidian Distance) of the start location
void WrapGridAlgorithm::CircularBreadthFirstSearch
(
    const Location &startLoc_r, 
    const int radius2,
    function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
    function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationBfsDistance, directionTowardStart) -> interruptBfs
)
{
    // Initialize algorithm
    queue<Location> locQueue;
    vector<vector<int>> distances(Rows, vector<int>(Cols, -1));

    // Prime BFS' values for start location
    if (onVisited_f(startLoc_r, 0, -1)) // Stop BFS on the start location if needed
    {
        return;
    }
    locQueue.push(startLoc_r);
    distances[startLoc_r.Row][startLoc_r.Col] = 0;

    // Executes main BFS Algorithm
    Location currLoc, nextLoc;
    int nextDist;

    //    Executes BFS until no more valid locations can be reached
    while (!locQueue.empty())
    {
        // Retrieve a location
        currLoc = locQueue.front();
        locQueue.pop();
        nextDist = distances[currLoc.Row][currLoc.Col] + 1;

        // Add valid neighbours to the queue
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            // Retrieve neighbour
            nextLoc = GetLocation(currLoc, d);

            // Add neighbour to the queue if it is valid
            if ((distances[nextLoc.Row][nextLoc.Col] == -1) &&
                (validLocationPredicate_f(nextLoc)) &&
                (Distance2(nextLoc, startLoc_r) <= radius2))
            {
                // Send visited location information, and exit early if needed
                if (onVisited_f(nextLoc, nextDist, (d + TDIRECTIONS / 2) % TDIRECTIONS))
                {
                    return;
                }
                locQueue.push(nextLoc);
            }

            distances[nextLoc.Row][nextLoc.Col] = nextDist;
        }
    }
}

// Executes a BFS using a base location queue and a base distances grid
void WrapGridAlgorithm::BreadthFirstSearch
(
    queue<Location> &locQueue_r,
    vector<vector<int>> &distances_r,
    const int range,
    function<bool(const Location&)> const &validLocationPredicate_f, // (currentLocation) -> isValid
    function<bool(const Location&, const int, const int)> const &onVisited_f // (currentLocation, locationDistance, directionTowardStart) -> interruptBfs
)
{
    Location currLoc, nextLoc;
    int nextDist;

    // Executes BFS until no more valid locations can be reached
    while (!locQueue_r.empty())
    {
        // Retrieve a location
        currLoc = locQueue_r.front();
        locQueue_r.pop();
        nextDist = distances_r[currLoc.Row][currLoc.Col] + 1;

        // Add valid neighbours to the queue
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            // Retrieve neighbour
            nextLoc = GetLocation(currLoc, d);

            // Add neighbour to the queue if it is valid
            if ((distances_r[nextLoc.Row][nextLoc.Col] == -1) &&
                (validLocationPredicate_f(nextLoc)) &&
                (nextDist <= range))
            {
                // Send visited location information, and exit early if needed
                if (onVisited_f(nextLoc, nextDist, (d + TDIRECTIONS / 2) % TDIRECTIONS))
                {
                    return;
                }
                locQueue_r.push(nextLoc);
            }

            distances_r[nextLoc.Row][nextLoc.Col] = nextDist;
        }
    }
}