#include "CombatEvaluator.h"

#include <limits>
#include <unordered_map>

#include "State.h"
#include "WrapGridAlgorithm.h"

// Maximum number of iterations allowed for evaluating 
// the best move for a combat between a group of
// allies and a group of enemies
#define MAX_ITERATIONS 50000

using namespace std;

// Creates an evaluator for combats and give it the pointer to the game's state
CombatEvaluator::CombatEvaluator(State *gameState)
    : GameState(gameState)
{
    
}

// Initializes the combat state
void CombatEvaluator::Initialize(CombatState *startingCombatState)
{
    CurrentCombatState = startingCombatState;

    while (!BestMoves.empty())
        BestMoves.pop();

    BestGain = numeric_limits<int>::min();
    TotalIterations = 0;
}

// Attempts to computes the best move for a given starting combat state
// where one ally group combats one enemy group.
// The algorithm used is an adaptation of the alpha-beta algorithm, with limited iterations.
// The objective is minimizing the possible enemy gain while making ally moves.
void CombatEvaluator::ComputeBestMove(CombatState *startingCombatState)
{
    // Initializes the combat evaluation with the given starting state
    Initialize(startingCombatState);
    // Attempts to compute the best moves for allies within
    // the iteration count constraints
    ComputeNextAllyMove();
}

// Attempts to compute all possible moves for remaining unmoved allies
void CombatEvaluator::ComputeNextAllyMove()
{
    // Increments iteration count and skip computation if 
    // too many iterations were made
    TotalIterations++;
    if (TotalIterations > MAX_ITERATIONS) 
    {
        return;
    }
    
    // If there are unmoved allies, evaluate their best moves
    if (CurrentCombatState->UnmovedAllies.size() > 0)
    {
        // Initialization
        Ant *toMove = CurrentCombatState->UnmovedAllies.top();
        CurrentCombatState->UnmovedAllies.pop();

        // Store number of iterations for pseudo-randomness
        int iterations = TotalIterations;

        int move;
        Location nextLocation;
        bool moveMade = false;

        // Start computation
        for(int i = -1; i < TDIRECTIONS; i++)
        {
            // Compute next possible move (None, N, E, S, W)
            // using the iteration count as pseudo-randomness
            move = ((i + 1 + iterations) % (TDIRECTIONS + 1)) - 1;
            // Get next location for iterations' move
            nextLocation = 
                (move == -1) ? 
                (toMove->CurrentLocation) :
                WrapGridAlgorithm::GetLocation(toMove->CurrentLocation, move);
            
            // Check next location validity (no Water & no moved Ally)
            if ((GameState->Grid[nextLocation.Row][nextLocation.Col].IsWater) || 
                (CurrentCombatState->MovedAllyLocations.count(nextLocation) > 0))
            {
                continue;
            }
            // Change flag to tell that at least one possible move was found
            moveMade = true;

            // Register current move
            CurrentCombatState->AlliesPerformedMoves.push(pair(toMove, move));
            CurrentCombatState->MovedAllyLocations.insert(nextLocation);

            // Compute remaining moves
            ComputeNextAllyMove();
            
            // Revert current Move
            CurrentCombatState->MovedAllyLocations.erase(nextLocation);
            CurrentCombatState->AlliesPerformedMoves.pop();
        }
        
        // Don't move if no move is possible without colliding with an ally
        if (!moveMade) 
        {
            nextLocation = toMove->CurrentLocation;
            // Register current move (not moving)
            CurrentCombatState->AlliesPerformedMoves.push(pair(toMove, -1));
            CurrentCombatState->MovedAllyLocations.insert(nextLocation);

            // Compute remaining moves
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
        // Update best moves to do if moves with better gains were found
        int gain = ComputeNextEnemyMove();
        if (gain > BestGain)
        {
            BestGain = gain;
            BestMoves = CurrentCombatState->AlliesPerformedMoves;
        }
    }
}

// Attempts to computes all possible moves for remaining unmoved enemies
// and attempts to return the best possible enemy gain for given configuration
int CombatEvaluator::ComputeNextEnemyMove()
{
    // Increments iteration count and skip computation if 
    // too many iterations were made
    TotalIterations++;
    if (TotalIterations > MAX_ITERATIONS) 
    {
        // The default gain is mildly bad, when no more iterations can be made
        return -5;
    }

    // If there are unmoved enemies, evaluate their best moves
    if (CurrentCombatState->UnmovedEnemies.size() > 0)
    {
        // Initialization
        Ant *toMove = CurrentCombatState->UnmovedEnemies.top();
        CurrentCombatState->UnmovedEnemies.pop();

        // Store number of iterations for pseudo-randomness
        int iterations = TotalIterations;

        // Lowest possible ally gain for current enemy move
        int currentLowestGain = numeric_limits<int>::max();

        Location nextLocation;
        bool moveMade = false;
        int move;

        // Start computation
        for(int i = -1; i < TDIRECTIONS; i++)
        {
            // Compute next possible move (None, N, E, S, W)
            // using the iteration count as pseudo-randomness
            move = ((i + 1 + iterations) % (TDIRECTIONS + 1)) - 1;
            // Get next location for iterations' move
            nextLocation = 
                (move == -1) ? 
                (toMove->CurrentLocation) :
                WrapGridAlgorithm::GetLocation(toMove->CurrentLocation, move);
            
            // Check next location validity (no Water & no moved Ally)
            if ((GameState->Grid[nextLocation.Row][nextLocation.Col].IsWater) || 
                (CurrentCombatState->MovedEnemyLocations.count(nextLocation) > 0))
            {
                continue;
            }
            // Change flag to tell that at least one possible move was found
            moveMade = true;

            // Attempt Move
            CurrentCombatState->MovedEnemyLocations.insert(nextLocation);

            // Compute gain for configuration
            int currentGain = ComputeNextEnemyMove();

            // Revert Move
            CurrentCombatState->MovedEnemyLocations.erase(nextLocation);

            // If ally gain is worse than the best possible ally gain, then
            // this branch can be pruned
            if (currentGain < BestGain)
            {
                CurrentCombatState->UnmovedEnemies.push(toMove);
                return numeric_limits<int>::min();
            }

            // If ally gain is worse than current lowest gain
            // (= better enemy moves), register the gain value
            if (currentGain < currentLowestGain)
            {
                currentLowestGain = currentGain;
            }
        }

        // Don't move if no move is possible without colliding with an enemy
        if (!moveMade) 
        {
            nextLocation = toMove->CurrentLocation;
            // Attempt Move (not moving)
            CurrentCombatState->MovedEnemyLocations.insert(nextLocation);
            // Compute gain for configuration
            currentLowestGain = ComputeNextEnemyMove();
            // Revert current Move
            CurrentCombatState->MovedAllyLocations.erase(nextLocation);
        }

        // Revert
        CurrentCombatState->UnmovedEnemies.push(toMove);

        // Return best enemy gain (lowest ally gain)
        return currentLowestGain;
    }
    else
    {
        // If branch is terminal, evaluate it
        return EvaluateCurrentCombatState(); 
    }
}

// Evaluates a combat state's gain       
int CombatEvaluator::EvaluateCurrentCombatState()
{
    // Create counters used for computation
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
            if (WrapGridAlgorithm::Distance2(allyLoc, enemyLoc) > GameState->AttackRadius2)
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
            if (WrapGridAlgorithm::Distance2(allyLoc, enemyLoc) > GameState->AttackRadius2)
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
            if (WrapGridAlgorithm::Distance2(enemyLoc, allyLoc) > GameState->AttackRadius2)
                continue;
            
            if (EnemiesOpponentCount[enemyLoc] >= AlliesOpponentCount[allyLoc])
            {
                enemyDeathCount++;
                break;
            }
        }
    }

    // Compute gain 
    // (making an ally survive is more valuable than killing one enemy)
    return (enemyDeathCount * 10) - (allyDeathCount * 13); 
}
