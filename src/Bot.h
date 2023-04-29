#ifndef BOT_H_
#define BOT_H_

#include <unordered_map>
#include <vector>

#include "State.h"
#include "Location.h"
#include "GuardHillTask.h"

/*
    This struct represents your bot in the game of Ants
*/
class Bot
{
    public:
        State State;

        Bot();

        void PlayGame(); // Plays a single game of Ants
        void MakeMove(Ant* ant); // Makes move for a single ant
        void MakeMoves(); // Makes moves for a single Turn
        void EndTurn(); // Indicates to the engine that it has made its moves

    private:
        // std::unordered_map<int, JoinAntTask> _joinAntTasks;
        // std::unordered_map<Location, AttackHillTask> _attackHillTasks;
        std::vector<GuardHillTask> _guardHillTasks;
        // Ants which are currently blocked by other ants (key is blocking ant's ID, value is blocked ant)
        std::unordered_map<int, Ant*> _antsBlockedByOtherAnts;

        void MoveClosestAvailableAntTowards(const Location &targetLocation, int searchRadius);
        void SeekFood();
        void ExploreFog();
        void DestroyOtherHills();
        void Combat();

        // Create and assign tasks to ants
        void InitializeTasks();
        // Make Ants perform tasks
        void DoTasks();
        void ClearFinishedTasks();
};

#endif // BOT_H_
