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

    // Remove all moves
    MovedAnts.clear();

    SeekFood();

    //picks out moves for each ant
    for (int ant=0; ant<(int)State.MyAnts.size(); ant++)
    {
        // Check if ant already moved
        if (std::find(MovedAnts.begin(), MovedAnts.end(), State.MyAnts[ant]) == MovedAnts.end())
        {
            for (int d=0; d<TDIRECTIONS; d++)
            {
                Location loc = State.GetLocation(State.MyAnts[ant], d);

                if ((!State.Grid[loc.Row][loc.Col].IsWater) &&
                    (State.Grid[loc.Row][loc.Col].Ant.Team == -1))
                {
                    MakeMove(State.Grid[State.MyAnts[ant].Row][State.MyAnts[ant].Col].Ant, d);
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

    for (int food = 0; food <(int)State.Food.size(); food++)
    {
        antLocation = State.BreadthFirstSearch(
            State.Food[food],
            &direction, 
            (int)State.ViewRadius,
            [this](const Location& location)
            {
                return 
                    (State.Grid[location.Row][location.Col].Ant.Team == 0) &&
                    (!State.Grid[location.Row][location.Col].Ant.Decided) &&
                    (std::find(MovedAnts.begin(), MovedAnts.end(), location) == MovedAnts.end());
            }
        );
        
        if (!(antLocation == Location(-1,-1)))
        {
            MakeMove(State.Grid[antLocation.Row][antLocation.Col].Ant, direction);
            MovedAnts.push_back(antLocation);
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
    State.Grid[nLoc.Row][nLoc.Col].Ant.Decided = ant.Decided;
    State.Grid[nLoc.Row][nLoc.Col].Ant.Team = ant.Team;
    State.Grid[nLoc.Row][nLoc.Col].Ant.NextLocation = State.Grid[nLoc.Row][nLoc.Col].Ant.CurrentLocation;
}
