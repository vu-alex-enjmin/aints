#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <stdint.h>
#include <functional>

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
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

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
    double LoadTime, TurnTime;
    std::vector<double> Scores;
    bool GameOver;
    int64_t Seed;

    std::vector<std::vector<Square>> Grid;
    std::vector<Location> MyAnts, EnemyAnts, MyHills, EnemyHills, Food;

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
    double ManhattanDistance(const Location &loc1, const Location &loc2);
    Location GetLocation(const Location &startLoc, int direction);
    Location BreadthFirstSearch(const Location &startLoc, int* outDirection, int range, std::function<bool(const Location&)> const &stopPredicate);
    // Returns the directions that need to be followed to reach target. 
    // Directions are int values: (0 for 'N', 1 for 'E', 2 for 'S', 3 for 'W')
    std::vector<int> AStar(const Location &startLoc, const Location &targetLoc);
    Location SearchMostFogged(const Location &startLoc, int* outDirection, int stopRange);
    void UpdateVisionInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif //STATE_H_
