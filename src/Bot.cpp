#include "Bot.h"

#include <unordered_set>
#include <algorithm>
#include <cstdlib>

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

    srand(State.Seed);

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
    State.Bug << "Combat" << endl;
    Combat();   
    State.Bug << "DestroyOtherHills" << endl; 
    DestroyOtherHills();
    State.Bug << "SeekFood" << endl;
    SeekFood();
    State.Bug << "ExploreFog" << endl;
    ExploreFog();
    State.Bug << "FinalMove" << endl;
    int offset;
    int direction;

    Ant* ant;
    //picks out moves for each ant
    for (const auto antPair : State.MyIndexedAnts)
    {
        ant = antPair.second;
        // Check if ant already moved
        if (!ant->Decided)
        {
            offset = rand();
            for (int d=0; d<TDIRECTIONS; d++)
            {
                direction = ( d + offset ) % TDIRECTIONS;
                Location destination = State.GetLocation(ant->CurrentLocation, direction);

                if ((!State.Grid[destination.Row][destination.Col].IsWater) &&
                    (State.Grid[destination.Row][destination.Col].Ant == nullptr) &&
                    (State.Grid[destination.Row][destination.Col].HillPlayer != 0))
                {
                    MakeMove(ant, direction);
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
                (State.Grid[location.Row][location.Col].Ant != nullptr) &&
                (State.Grid[location.Row][location.Col].Ant->Team == 0) &&
                (!State.Grid[location.Row][location.Col].Ant->Decided);
        }
    );
    
    if (!(antLocation == Location(-1,-1)))
    {
        Location newLocation = State.GetLocation(antLocation, direction);
        if(State.Grid[newLocation.Row][newLocation.Col].Ant == nullptr)
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
        if (visitedSquare.Ant != nullptr &&
            visitedSquare.Ant->Team == 0 &&
            !visitedSquare.Ant->Decided)
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


void Bot::ExploreFog()
{
    int direction;
    Location destination;
    
    Ant* ant;
    //picks out moves for each ant
    for (const auto antPair : State.MyIndexedAnts)
    {
        ant = antPair.second;
        // Check if ant already moved
        if (!ant->Decided)
        {
            State.Bug << "Enter SearchMostFogged"<<endl;

            destination = State.SearchMostFogged(ant->CurrentLocation, &direction, ((int)State.ViewRadius)+5);
            if (destination != Location(-1,-1))
            {
                MakeMove(ant, direction);
            }

            State.Bug << "Exit SearchMostFogged"<<endl;
        }else{
            State.Bug << "Ant ("<< ant->Id<<") at " << ant->CurrentLocation.Row <<
                "/" << ant->CurrentLocation.Col << "already decided" <<endl;
        }
        
    }
}

// outputs move information to the engine
// and registers move info in ant
void Bot::MakeMove(Ant* ant, int direction)
{
    Location nLoc = State.GetLocation(ant->CurrentLocation, direction);
    if(State.Grid[nLoc.Row][nLoc.Col].IsFood)
    {
        // Do not move if going towards food (food has collisions)
        ant->Decided = true;
        ant->NextLocation = ant->CurrentLocation;
    }
    else
    {
        cout << "o " << ant->CurrentLocation.Row << " " << ant->CurrentLocation.Col << " " << CDIRECTIONS[direction] << endl;
        State.Bug << "Moving ant of ID " << ant->Id << " to " << nLoc.Row << "/" << nLoc.Col << endl;
        ant->NextLocation = nLoc;
        ant->Decided = true;
        ant->MoveDirection = direction;
        // Add destination Ant to Grid
        State.Grid[nLoc.Row][nLoc.Col].Ant = ant;
    }
}
