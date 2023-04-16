#include "Bot.h"

#include <unordered_set>
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

    Combat();    
    DestroyOtherHills();
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

void Bot::MoveClosestAvailableAntTowards(const Location &targetLocation, const int searchRadius)
{
    int direction;
    Location antLocation;

    antLocation = State.BreadthFirstSearch(
        targetLocation,
        &direction, 
        searchRadius,
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

void Bot::SeekFood()
{
    for (Location &foodLoc : State.Food)
    {
        MoveClosestAvailableAntTowards(foodLoc, (int)State.ViewRadius);
    }
}

void Bot::DestroyOtherHills()
{
    for (Location &hillLoc : State.EnemyHills)
    {
        MoveClosestAvailableAntTowards(hillLoc, (int)(2 * State.ViewRadius));
    }
}

void Bot::Combat()
{
    if (State.EnemyAnts.empty())
        return;

    // Initialize
    vector<unordered_set<Location, Location>> allyGroups;
    vector<unordered_set<Location, Location>> enemyGroups;
    unordered_set<Location, Location> allyGroup;
    auto onVisited = [&,this](const Location &location)
    {
        Square &visitedSquare = State.Grid[location.Row][location.Col];
        if (visitedSquare.Ant.Team == 0 && !visitedSquare.Ant.Decided)
        {
            allyGroup.insert(location);
        }
    };
    
    // Search for enemy groups close to allies
    for (const Location &antLoc : State.EnemyAnts)
    {
        allyGroup.clear();
        State.BreadthFirstSearchAll(antLoc, State.AttackRadius + 2, onVisited, true);
        
        if (allyGroup.size() > 0)
        {
            unordered_set<Location, Location> allyGroupCopy = allyGroup;
            allyGroups.push_back(allyGroupCopy);

            unordered_set<Location, Location> enemyGroup;
            enemyGroup.insert(antLoc);
            enemyGroups.push_back(enemyGroup);
        }
    }

    // Merge groups that are close to each-other to create "armies"
    for (int i = 0; i < allyGroups.size(); i++)
    {
        bool merge = false;
        for (int j = 0; j < allyGroups.size(); j++)
        {
            if (i == j)
                continue;
            
            // Check whether a merge should be made
            merge = false;
            for (const Location &allyLoc : allyGroups[i])
            {
                if (allyGroups[j].count(allyLoc) > 0)
                {
                    merge = true;
                    break;
                }
            }

            // Merge if needed
            if (merge)
            {
                allyGroups[i].insert(allyGroups[j].begin(), allyGroups[j].end());
                allyGroups.erase(allyGroups.begin() + j);
                enemyGroups[i].insert(enemyGroups[j].begin(), enemyGroups[j].end());
                enemyGroups.erase(enemyGroups.begin() + j);

                if (j < i)
                    i--;
                
                j--;
            }
        }
    }

    // TODO : compute best combat strategy for each enemy/ally armies pair
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
