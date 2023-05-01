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
// TODO : move to bot
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};

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

    // Check if pointer points to an ally ant that has not moved yet
    bool IsAvailableAnt(const Ant *ant);
    // Check if location in the Grid is an ally ant that has not moved yet
    bool IsAvailableAnt(const Location& location);
    // Check if id is a live ally ant that has not moved yet
    bool IsAvailableAnt(const int id);

    // Update Grid Fog of War information
    void UpdateVisionInformation();
    // Update information related to Hills' existence
    void UpdateHillInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif // STATE_H_
