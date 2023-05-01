#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
// TODO : cleanup includes in header files
// TODO : sort includes in all files

#include <vector>
#include <queue>
#include <stack>
#include <stdint.h>
#include <functional>
#include <map>
#include <unordered_set>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "Ant.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // {N, E, S, W}

/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */
    int Rows, Cols,
        Turn, MaxTurns,
        NoPlayers;
    double AttackRadius, SpawnRadius, ViewRadius;
    int AttackRadius2, SpawnRadius2, ViewRadius2;
    double LoadTime, TurnTime;
    std::vector<double> Scores;
    bool GameOver;
    int64_t Seed;

    std::map<int, Ant*> AllyAnts;
    std::vector<std::vector<Square>> Grid;
    std::vector<Location> EnemyAnts, Food;
    std::unordered_set<Location, Location> MyHills, EnemyHills;

    Timer Timer;
    Bug Bug;

    /*
        Functions
    */
    State();
    ~State();

    void Setup();
    void Reset();

    double Distance(const Location &loc1, const Location &loc2);
    int Distance2(const Location &loc1, const Location &loc2);
    double ManhattanDistance(const Location &loc1, const Location &loc2);
    Location GetLocation(const Location &startLoc, int direction);
    Location BreadthFirstSearch(const Location &startLoc, int* outDirection, int range, std::function<bool(const Location&)> const &stopPredicate, bool ignoreWater = false);
    void CircularBreadthFirstSearchAll(const Location &startLoc, int range2, std::function<void(const Location&)> const &onVisited, bool ignoreWater = false);
    void MultiBreadthFirstSearchAll(const std::vector<Location> &startLocs, int range, std::function<bool(const Location&, const int, const int)> const &onVisited, bool ignoreWater = false);
    // Returns the directions that need to be followed to reach target. 
    // Directions are int values: (0 for 'N', 1 for 'E', 2 for 'S', 3 for 'W')
    std::vector<int> AStar(const Location &startLoc, const Location &targetLoc);
    Location SearchMostFogged(const Location &startLoc, int* outDirection, int stopRange);

    // Check if pointer points to an ally ant that has not moved yet
    bool IsAvailableAnt(const Ant* ant);
    // Check if location in the Grid is an ally ant that has not moved yet
    bool IsAvailableAnt(const Location& location);
    // Check if id is a live ally ant that has not moved yet
    bool IsAvailableAnt(const int id);

    void UpdateVisionInformation();
    void UpdateHillInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif // STATE_H_
