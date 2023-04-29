#ifndef COMBAT_EVALUATOR_H_
#define COMBAT_EVALUATOR_H_

struct State;
#include "CombatState.h"

class CombatEvaluator
{    
    public:
        State *GameState;
        CombatState *CurrentCombatState;
        std::stack<std::pair<Ant*, int>> BestMoves;
        int BestGain;
        int TotalIterations;

        CombatEvaluator(State *gameState);
        void ComputeBestMove(CombatState *startingCombatState);
        int EvaluateCurrentCombatState();

    private:
        void Initialize(CombatState *startingCombatState);
        void ComputeNextAllyMove();
        int ComputeNextEnemyMove();
};

#endif // COMBAT_EVALUATOR_H_