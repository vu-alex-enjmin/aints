#ifndef COMBAT_STATE_H_
#define COMBAT_STATE_H_

#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "Ant.h"

struct CombatState
{
    public:
        
        /*
        =========================================
            Attributes
        =========================================
        */
       
        // Moves performed by allies to reach current state
        std::stack<std::pair<Ant*, int>> AlliesPerformedMoves;
        // Allies which have not made their move yet
        std::stack<Ant*> UnmovedAllies;
        // Enemies which have not made their move yet
        std::stack<Ant*> UnmovedEnemies;
        // Location of allies after they've moved
        std::unordered_set<Location, Location> MovedAllyLocations;
        // Location of enemies after they have made their move
        std::unordered_set<Location, Location> MovedEnemyLocations;

        /*
        =========================================
            Methods
        =========================================
        */

        CombatState() = default;
};
#endif // COMBAT_STATE_H_