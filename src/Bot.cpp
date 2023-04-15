#include "Bot.h"
#include <algorithm>
using namespace std;

//constructor
Bot::Bot()
{

}

//plays a single game of Ants.
void Bot::PlayGame()
{
    //reads the game parameters and sets up
    cin >> State;
    State.Setup();
    EndTurn();

    //continues making moves while the game is not over
    while(cin >> State)
    {
        State.UpdateVisionInformation();
        MakeMoves();
        EndTurn();
    }
}

//makes the bots moves for the Turn
void Bot::MakeMoves()
{
    State.Bug << "Turn " << State.Turn << ":" << endl;
    State.Bug << State << endl;

    // Remove all moves
    MovedAnts.clear();

    int direction;
    Location antLocation;

    for(int food = 0; food <(int)State.Food.size(); food++){
        antLocation = State.BreadthFirstSearch(State.Food[food],
        &direction, (int)State.ViewRadius,
        [](const Square& square){
            return square.Ant == 0;
        });
        if(!(antLocation == Location(-1,-1)) &&
        std::find(MovedAnts.begin(), MovedAnts.end(), antLocation) == MovedAnts.end()){
            State.MakeMove(antLocation, direction);
            MovedAnts.push_back(antLocation);
        }
    }

    //picks out moves for each ant
    for(int ant=0; ant<(int)State.MyAnts.size(); ant++)
    {
        // Check if ant already moved
        if(std::find(MovedAnts.begin(), MovedAnts.end(), State.MyAnts[ant]) == MovedAnts.end())
        {
            for(int d=0; d<TDIRECTIONS; d++)
            {
                Location loc = State.GetLocation(State.MyAnts[ant], d);

                if(!State.Grid[loc.Row][loc.Col].IsWater)
                {
                    State.MakeMove(State.MyAnts[ant], d);
                    break;
                }
            }
        }
    }

    State.Bug << "time taken: " << State.Timer.GetTime() << "ms" << endl << endl;
}

//finishes the Turn
void Bot::EndTurn()
{
    if(State.Turn > 0)
        State.Reset();
    State.Turn++;

    cout << "go" << endl;
}
