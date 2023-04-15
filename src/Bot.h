#ifndef BOT_H_
#define BOT_H_

#include "State.h"

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    State State;
    vector<Location> MovedAnts;

    Bot();

    void PlayGame();    //plays a single game of Ants

    void MakeMoves();   //makes moves for a single Turn
    void EndTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
