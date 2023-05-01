#ifndef BOT_H_
#define BOT_H_

#include <unordered_map>
#include <vector>

#include "State.h"
#include "Location.h"
#include "GuardHillTask.h"
#include "ReachLocationTask.h"
#include "ReachAntTask.h"
#include "CombatEvaluator.h"

// Move directions, represented by characters.
// Used for telling game engine of the moves to make. 
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};

/*
    This struct represents the bot (AI) in the game of Ants
*/
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
        // Makes move for a single ant
        void MakeMove(Ant *ant);
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

        // tasks for forming a barrier around hills
        std::vector<GuardHillTask> _guardHillTasks;
        // tasks for attacking invaders inside barriers around hills
        std::vector<ReachLocationTask> _defendHillTasks;
        // tasks for joining an ant in combat
        std::unordered_map<int, std::vector<ReachAntTask*>> _allyReinforcementTasks;
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
        void MoveClosestAvailableAntsTowards(const Location &targetLocation, int searchRange, int maxAnts = 1);
    
        // Orders ants to look for and go towards nearby food
        void SeekFood();
        // Orders ants to go to places that are in the Fog Of War
        void ExploreFog();
        // Orders ants go to hills to destroy them
        void DestroyOtherHills();
        
        // Identifies occuring battles and groups the allies and enemies of each battle 
        void ComputeArmies();
        // Makes groups of allies formed by ComputeArmies() move in order to resolve battles
        void Combat();
        
        // Orders ants to follow nearby enemies
        void ApproachEnemies();


        // >> Task Management functions

        // Creates and assign tasks to ants
        void InitializeTasks();
        // Creates and assign tasks related to guarding and defending the hill
        void InitializeGuardHillTasks();
        // Creates and assign All
        // TODO commenter si on l'utilise ou pas
        void InitializeAllyReinforcementTasks();
        
        // Makes Ants perform tasks
        void DoTasks();
        // Sends ants to protect an ally hill from approaching enemies
        void DefendHills();
        // Remove all tasks that are either completed or irrelevant
        void ClearFinishedTasks();
};

#endif // BOT_H_
