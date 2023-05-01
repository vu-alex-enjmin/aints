#ifndef COMBAT_EVALUATOR_H_
#define COMBAT_EVALUATOR_H_

struct State;
#include "CombatState.h"

// Class used to evaluate the best move for a given combat configuration
class CombatEvaluator
{    
    public:
        /*
        =========================================
            Attributes
        =========================================
        */
        
        // Pointer to the game's current state
        State *GameState;
        // Pointer to the combat state which is being evaluated
        CombatState *CurrentCombatState;
        // Stack of the best combination of moves which have been found so far
        // in the format <point to Ant, move direction index>
        std::stack<std::pair<Ant*, int>> BestMoves;
        // Best evaluated gain found so far
        int BestGain;
        // Number of iterations made after initialization a new combat
        int TotalIterations;

        /*
        =========================================
            Methods
        =========================================
        */

        // Creates an evaluator for combats and give it the pointer to the game's state
        CombatEvaluator(State *gameState);
        // Attempts to computes the best move for a given starting combat state
        // where one ally group combats one enemy group.
        // The algorithm used is an adaptation of the alpha-beta algorithm, with limited iterations.
        // The objective is minimizing the possible enemy gain while making ally moves.
        void ComputeBestMove(CombatState *startingCombatState);
        // Evaluates a combat state's gain
        int EvaluateCurrentCombatState();

    private:
        /*
        =========================================
            Methods
        =========================================
        */

        // Initializes the combat state
        void Initialize(CombatState *startingCombatState);
        // Attempts to compute all possible moves for remaining unmoved allies
        void ComputeNextAllyMove();
        // Attempts to computes all possible moves for remaining unmoved enemies
        // and attempts to return the best possible enemy gain for given configuration
        int ComputeNextEnemyMove();
};

#endif // COMBAT_EVALUATOR_H_