#include "CombatEvaluator.h"

#include "State.h"

#include <limits>
#include <unordered_map>

using namespace std;

CombatEvaluator::CombatEvaluator(State *gameState)
    : GameState(gameState)
{
    
}

void CombatEvaluator::Initialize(CombatState *startingCombatState)
{
    CurrentCombatState = startingCombatState;

    while (!BestMoves.empty())
        BestMoves.pop();

    BestGain = numeric_limits<int>::min();
    TotalIterations = 0;
}
        
void CombatEvaluator::ComputeBestMove(CombatState *startingCombatState)
{
    GameState->Bug << "Initialize" << endl;
    Initialize(startingCombatState);
    GameState->Bug << "Initialize End - Begin ComputeNextAllyMove" << endl;
    ComputeNextAllyMove();
    GameState->Bug << "Algorithm End ("<< TotalIterations<<" Iterations)" << endl;

}
        
void CombatEvaluator::ComputeNextAllyMove()
{
    //GameState->Bug << " Compute Ally " << endl;
    
    TotalIterations++;
    // TODO : check if TotalIterations too high
    if (CurrentCombatState->UnmovedAllies.size() > 0)
    {
        //GameState->Bug << "UnmovedAllies > 0" << endl;
        /*
        if antIndex < myAnts.size
            myAnt = myAnts[antIndex]
            for each possible move of myAnt
                simulate move
                max(antIndex+1)
                undo move

        */
        Ant *toMove = CurrentCombatState->UnmovedAllies.top();
        //GameState->Bug << "Ant Exists? "<< (toMove != nullptr) << endl;
        CurrentCombatState->UnmovedAllies.pop();
        //GameState->Bug << "Popped " << endl;
        Location nextLocation;

        int iterations = TotalIterations;
        int move;
        bool moveMade = false;
        // for(int i = -1; i < TDIRECTIONS; i++)
        for(int i = 0; i < TDIRECTIONS; i++)
        {
            move = (i + iterations) % TDIRECTIONS;
            //GameState->Bug << "Ally Compute Next Location" << endl;
            nextLocation = 
                // (i == -1) ? 
                // (toMove->CurrentLocation) :
                GameState->GetLocation(toMove->CurrentLocation,move);
            
            //GameState->Bug << "Ally Check Location" << endl;
            // Check next location validity (not Water & no moved Ally)
            if ((GameState->Grid[nextLocation.Row][nextLocation.Col].IsWater) || 
                (CurrentCombatState->MovedAllyLocations.count(nextLocation) > 0))
            {
                //GameState->Bug << "Ally Continue" << endl;
                continue;
            }
            moveMade = true;

            CurrentCombatState->AlliesPerformedMoves.push(pair(toMove, move));
            CurrentCombatState->MovedAllyLocations.insert(nextLocation);

            //GameState->Bug << "Ally Compute Next" << endl;
            ComputeNextAllyMove();
            
            // Revert current Move
            CurrentCombatState->MovedAllyLocations.erase(nextLocation);
            CurrentCombatState->AlliesPerformedMoves.pop();
        }
        
        // Don't move if no move is possible without hitting anybody
        if (!moveMade) 
        {
            //GameState->Bug << "Ally No Move" << endl;
            
            nextLocation = toMove->CurrentLocation;
            CurrentCombatState->AlliesPerformedMoves.push(pair(toMove, -1));
            CurrentCombatState->MovedAllyLocations.insert(nextLocation);

            ComputeNextAllyMove();
            
            // Revert current Move
            CurrentCombatState->MovedAllyLocations.erase(nextLocation);
            CurrentCombatState->AlliesPerformedMoves.pop();
        }
        // Revert
        CurrentCombatState->UnmovedAllies.push(toMove);
    }
    else
    {
        //GameState->Bug << "No UnmovedAllies" << endl;
        /*
        else
        value = min(0)
        if value > bestValue
            bestValue = value
            save the current simulated moves of all my ants
        */
        
        int gain = ComputeNextEnemyMove();
        if (gain > BestGain)
        {
            //GameState->Bug << "Ally Best " << BestGain << " -> " << gain << endl; 
            BestGain = gain;
            BestMoves = CurrentCombatState->AlliesPerformedMoves;
        }
        /*
        else 
        {
            GameState->Bug << "No Gain (only pain)" << endl;
            GameState->Bug << ">Ally " << BestGain << " & " << gain << endl; 
        }
        */
    }
}

int CombatEvaluator::ComputeNextEnemyMove()
{
/*

if antIndex < enemyAnts.size
    minValue = +Infinity
    enemyAnt = enemyAnts[antIndex]
    for each possible move of enemyAnt
        simulate move
        value = min(antIndex+1)
        undo move

        if value < bestValue
            return -Infinity  // cut!

        if value < minValue
            minValue = value

    return minValue
else
    return evaluate()
*/

    GameState->Bug << " Compute Enemy " << endl;

    if (CurrentCombatState->UnmovedEnemies.size() > 0)
    {
        Ant *toMove = CurrentCombatState->UnmovedEnemies.top();
        CurrentCombatState->UnmovedEnemies.pop();

        int currentLowestGain = numeric_limits<int>::max();
        int currentGain;
        Location nextLocation;

        bool moveMade = false;
        // for(int i = -1; i < TDIRECTIONS; i++)
        for(int i = 0; i < TDIRECTIONS; i++)
        {
            nextLocation = 
                // (i == -1) ? 
                // (toMove->CurrentLocation) :
                (GameState->GetLocation(toMove->CurrentLocation, i));
            
            // Check next location validity (not Water & no moved Ally)
            if ((GameState->Grid[nextLocation.Row][nextLocation.Col].IsWater) || 
                (CurrentCombatState->MovedEnemyLocations.count(nextLocation) > 0))
            {
                continue;
            }
            moveMade = true;

            // Attempt Move
            CurrentCombatState->MovedEnemyLocations.insert(nextLocation);
            // Compute gain for configuration
            currentGain = ComputeNextEnemyMove();
            // Revert Move
            CurrentCombatState->MovedEnemyLocations.erase(nextLocation);

            if (currentGain < BestGain)
            {
                CurrentCombatState->UnmovedEnemies.push(toMove);
                return numeric_limits<int>::min();
            }

            if (currentGain < currentLowestGain)
            {
                currentLowestGain = currentGain;
            }
        }

        if (!moveMade) 
        {
            // Attempt Move
            nextLocation = toMove->CurrentLocation;
            CurrentCombatState->MovedEnemyLocations.insert(nextLocation);
            // Compute gain for configuration
            currentLowestGain = ComputeNextEnemyMove();
            // Revert current Move
            CurrentCombatState->MovedAllyLocations.erase(nextLocation);
        }

        // Revert
        CurrentCombatState->UnmovedEnemies.push(toMove);

        return currentLowestGain;
    }
    else
    {
        return EvaluateCurrentCombatState(); 
    }
}
        
int CombatEvaluator::EvaluateCurrentCombatState()
{
    std::unordered_map<const Location, int, Location> AlliesOpponentCount;
    std::unordered_map<const Location, int, Location> EnemiesOpponentCount;
    
    // Initialize opponent count for allies and enemies
    for (const auto &allyLoc : CurrentCombatState->MovedAllyLocations) 
    {
        AlliesOpponentCount[allyLoc] = 0;
    }
    for (const auto &enemyLoc : CurrentCombatState->MovedEnemyLocations) 
    {
        EnemiesOpponentCount[enemyLoc] = 0;
    }

    // Compute opponent count for allies and enemies
    for (const auto &allyLoc : CurrentCombatState->MovedAllyLocations) 
    {
        for (const auto &enemyLoc : CurrentCombatState->MovedEnemyLocations) 
        {
            if (GameState->Distance2(allyLoc, enemyLoc) > GameState->AttackRadius2)
                continue;
            
            AlliesOpponentCount[allyLoc]++;
            EnemiesOpponentCount[enemyLoc]++;
        }
    }

    // Compute how many ally ant will die
    int allyDeathCount = 0;
    for (const auto &allyLoc : CurrentCombatState->MovedAllyLocations) 
    {
        for (const auto &enemyLoc : CurrentCombatState->MovedEnemyLocations) 
        {
            if (GameState->Distance2(allyLoc, enemyLoc) > GameState->AttackRadius2)
                continue;
            
            if (AlliesOpponentCount[allyLoc] >= EnemiesOpponentCount[enemyLoc])
            {
                allyDeathCount++;
                break;
            }
        }
    }

    // Compute how many enemy ant will die
    int enemyDeathCount = 0;
    for (const auto &enemyLoc : CurrentCombatState->MovedEnemyLocations)
    {
        for (const auto &allyLoc : CurrentCombatState->MovedAllyLocations)
        {
            if (GameState->Distance2(enemyLoc, allyLoc) > GameState->AttackRadius2)
                continue;
            
            if (EnemiesOpponentCount[enemyLoc] >= AlliesOpponentCount[allyLoc])
            {
                enemyDeathCount++;
                break;
            }
        }
    }

    GameState->Bug << "Evaluate : " << ((enemyDeathCount * 10) - (allyDeathCount * 15)) << endl;
    
    return (enemyDeathCount * 10) - (allyDeathCount * 15); 
}
