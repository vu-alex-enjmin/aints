#ifndef BOT_H_
#define BOT_H_

#include <unordered_map>
#include <vector>

#include "State.h"
#include "Location.h"
#include "GuardHillTask.h"
#include "ReachLocationTask.h"
#include "CombatEvaluator.h"

/*
    This struct represents your bot in the game of Ants
*/
class Bot
{
    public:
        State State;
        CombatEvaluator CombatEvaluator;

        Bot();

        void PlayGame(); // Plays a single game of Ants
        void MakeMove(Ant* ant); // Makes move for a single ant
        void MakeMoves(); // Makes moves for a single Turn
        void EndTurn(); // Indicates to the engine that it has made its moves

    private:
        // Combat management
        std::vector<std::unordered_set<Location, Location>> allyGroups; // Groups of allies
        std::vector<std::unordered_set<Location, Location>> enemyGroups; // Groups of enemies opposing each ally group (same size as allyGroups)

        // std::unordered_map<int, JoinAntTask> _joinAntTasks;
        // std::unordered_map<Location, AttackHillTask> _attackHillTasks;
        std::vector<GuardHillTask> _guardHillTasks;
        std::vector<ReachLocationTask> _defendHillTasks;
        // Ants which are currently blocked by other ants (key is blocking ant's ID, value is blocked ant)
        std::unordered_map<int, Ant*> _antsBlockedByOtherAnts;
        std::vector<Location> _hillInvaderAnts; // Ants that are in the guardHill walls
        void MoveClosestAvailableAntTowards(const Location &targetLocation, int searchRadius);
        void SeekFood();
        void ExploreFog();
        void DestroyOtherHills();
        void DefendHills();
        void Combat();
        void ComputeArmies();

        // Create and assign tasks to ants
        void InitializeTasks();
        void InitializeGuardHillTasks();
        void InitializeAllyReinforcementTasks();

        // Make Ants perform tasks
        void DoTasks();
        void ClearFinishedTasks();
};

#endif // BOT_H_
