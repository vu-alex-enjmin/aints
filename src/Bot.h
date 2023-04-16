#ifndef BOT_H_
#define BOT_H_

#include <vector>
#include "State.h"

/*
    This struct represents your bot in the game of Ants
*/
class Bot
{
    public:
    State State;
    std::vector<Location> MovedAnts;

    Bot();

    void PlayGame();    //plays a single game of Ants

    void MakeMove(Ant& ant, int direction); // makes move for a single ant
    void MakeMoves();   //makes moves for a single Turn

    void EndTurn();     //indicates to the engine that it has made its moves

    private:
    void SeekFood();
};

#endif //BOT_H_
