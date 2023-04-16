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
    while (cin >> State)
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

    SeekFood();

    //picks out moves for each ant
    for (Location &antLoc : State.MyAnts)
    {
        // Check if ant already moved
        if (!State.Grid[antLoc.Row][antLoc.Col].Ant.Decided)
        {
            for (int d=0; d<TDIRECTIONS; d++)
            {
                Location loc = State.GetLocation(antLoc, d);

                if ((!State.Grid[loc.Row][loc.Col].IsWater) &&
                    (State.Grid[loc.Row][loc.Col].Ant.Team == -1))
                {
                    MakeMove(State.Grid[antLoc.Row][antLoc.Col].Ant, d);
                    break;
                }
            }
        }
    }

    State.Bug << "time taken: " << State.Timer.GetTime() << "ms" << endl << endl;
}

void Bot::SeekFood()
{
    int direction;
    Location antLocation;

    for (Location &foodLoc : State.Food)
    {
        antLocation = State.BreadthFirstSearch(
            foodLoc,
            &direction, 
            (int)State.ViewRadius,
            [this](const Location& location)
            {
                return 
                    (State.Grid[location.Row][location.Col].Ant.Team == 0) &&
                    (!State.Grid[location.Row][location.Col].Ant.Decided);
            }
        );
        
        if (!(antLocation == Location(-1,-1)))
        {
            MakeMove(State.Grid[antLocation.Row][antLocation.Col].Ant, direction);
        }
    }
}

//finishes the Turn
void Bot::EndTurn()
{
    if (State.Turn > 0)
        State.Reset();
    State.Turn++;

    cout << "go" << endl;
}


// outputs move information to the engine
// and registers move info in ant
void Bot::MakeMove(Ant& ant, int direction)
{
    cout << "o " << ant.CurrentLocation.Row << " " << ant.CurrentLocation.Col << " " << CDIRECTIONS[direction] << endl;

    Location nLoc = State.GetLocation(ant.CurrentLocation, direction);

    ant.NextLocation = nLoc;
    ant.Decided = true;
    ant.MoveDirection = direction;

    // Add destination Ant to Grid
    State.Grid[nLoc.Row][nLoc.Col].Ant = ant;
}
