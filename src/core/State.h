#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "Ant.h"

// Struct to store current game state information
struct State
{
    public:
        /*
        =========================================
            Attributes
        =========================================
        */
        // Number of rows of game grid
        int Rows;
        // Number of columns of game grid
        int Cols;
        // Current game turn
        int Turn;
        // Maximum number of turns for whole game
        int MaxTurns;
        // Number of players for the game
        int NoPlayers;
        // Attack radius of ants
        double AttackRadius;
        // Attack radius of ants squared
        int AttackRadius2;
        // Gathering radius of food
        double SpawnRadius;
        // Gathering radius of food squared
        int SpawnRadius2; 
        // View radius of ants
        double ViewRadius;
        // View radius of ants squared
        int ViewRadius2;
        // Time allowed for processing game data
        double LoadTime;
        // Time allowed for giving instructions for a turn
        double TurnTime;
        // Scores of each player
        std::vector<double> Scores;
        // Whether the game is over (true if it is, false otherwise)
        bool GameOver;
        // Seed for Bot's RNG
        int64_t Seed;
        // Ally ants in the format <Ant Id, pointer to Ant>
        std::map<int, Ant*> AllyAnts;
        // 2D Grid containing each square's data
        std::vector<std::vector<Square>> Grid;
        // Location of visible enemies
        std::vector<Location> EnemyAnts;
        // Location of visible food
        std::vector<Location> Food;
        // Locations which should contain ally hills (could be false if out of view radius)
        std::unordered_set<Location, Location> MyHills;
        // Locations which should contain enemy hills (could be false if out of view radius)
        std::unordered_set<Location, Location> EnemyHills;
        // Simple timer, used for checking computation time
        Timer Timer;
        // Object used for debug logs
        Bug Bug;

        /*
        =========================================
            Methods
        =========================================
        */
        // Create state while opening Debug logger if needed 
        State();
        // Destroy state while closing Debug logger if needed 
        ~State();

        // Sets the state up, with game start's information
        void Setup();
        // Resets all unmanaged containers, all non-water squares,
        // and update all managed containers
        void Reset();
        // Update Grid Fog of War information
        void UpdateVisionInformation();
        // Update information related to Hills' existence
        void UpdateHillInformation();
        // Check if pointer points to an ally ant that has not moved yet
        bool IsAvailableAnt(const Ant *ant_p);
        // Check if location in the Grid is an ally ant that has not moved yet
        bool IsAvailableAnt(const Location& location);
        // Check if id is a live ally ant that has not moved yet
        bool IsAvailableAnt(const int id);
};

// Output function for state, used for representing state inside output stream
std::ostream& operator<<(std::ostream &os_r, const State &state_r);
// Input function, used to initialize/update state
// after receiving game information
std::istream& operator>>(std::istream &is_r, State &state_r);

#endif // STATE_H_
