#ifndef BOT_H_
#define BOT_H_

#include <unordered_map>
#include <vector>

#include "State.h"
#include "Location.h"
#include "GuardHillTask.h"
#include "ReachLocationTask.h"
#include "CombatEvaluator.h"

// Move directions, represented by characters.
// Used for telling game engine of the moves to make. 
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};

// This class represents the bot (AI) in the game of Ants
class Bot
{
    public:
        /*
        =========================================
            Attributes
        =========================================
        */
       
        State State;
        CombatEvaluator CombatEvaluator;

        /*
        =========================================
            Methods
        =========================================
        */

        Bot();
        // Plays a single game of Ants
        void PlayGame();
        // Makes moves for a single Turn
        void MakeMoves();
        // Indicates to the engine that it has made its moves
        void EndTurn();

    private:
        /*
        =========================================
            Attributes
        =========================================
        */
        // >> Combat management attributes

        // Groups of allies in battles
        std::vector<std::unordered_set<Location, Location>> allyGroups;
        // Groups of enemies opposing each ally group (same size as allyGroups)
        std::vector<std::unordered_set<Location, Location>> enemyGroups;

        // >> Task management attributes 

        // Tasks for forming a barrier around hills
        std::vector<GuardHillTask> _guardHillTasks;
        // Tasks for attacking invaders inside barriers around hills
        // these tasks consists in making an ant rush towards the enemy at any cost
        std::vector<ReachLocationTask> _defendHillTasks;
        // Ants which are currently blocked by other ants
        // <blocking ant's ID, blocked ant>
        std::unordered_map<int, Ant*> _antsBlockedByOtherAnts;
        // Ants that are in the guardHill walls
        std::vector<Location> _hillInvaderAnts;

        /*
        =========================================
            Methods
        =========================================
        */
        // >> Simple Ant actions (No Tasks required) 

        // Orders maxAnts ants in searchRadius to move one step towards targetLocation
        // This general function is used by other functions
        void MoveClosestAvailableAntsTowards(const Location &targetLocation_r, int searchRange, int maxAnts = 1);
    
        // Orders ants to look for and go towards nearby food
        void SeekFood();
        // Orders ants to go to places that are in the Fog Of War
        void ExploreFog();
        // Orders ants to go to hills to destroy them
        void DestroyOtherHills();
        
        // Identifies occuring battles and groups the allies and enemies of each battle 
        void ComputeArmies();
        // Makes groups of allies formed by ComputeArmies() move in order to resolve battles
        void Combat();
        
        // Orders ants to follow nearby enemies
        void ApproachEnemies();

        // Orders ants to make a default move if they aren't doing anything else
        void MakeDefaultMove();
        // Move every ant that has chosen a move
        void ExecuteMoves();

        // >> Task Management functions

        // Creates and assign tasks to ants
        void InitializeTasks();
        // Creates and assign tasks related to guarding and defending the hill
        void InitializeHillProtectionTasks();
        // Makes Ants perform tasks
        void DoTasks();
        // Sends ants to protect an ally hill from approaching enemies
        void DefendHills();
        // Remove all tasks that are either completed or irrelevant
        void ClearFinishedTasks();

        // Make a single ant execute its move
        void MakeMove(Ant *ant_p);
};

#endif // BOT_H_
