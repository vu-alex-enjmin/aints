#include "Bot.h"

#include <unordered_set>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Constructor
Bot::Bot()
{

}

// Plays a single game of Ants.
void Bot::PlayGame()
{
    // Reads the game parameters and sets up
    cin >> State;
    State.Setup();
    EndTurn();

    srand(State.Seed);

    // Continues making moves while the game is not over
    while (cin >> State)
    {
        State.UpdateVisionInformation();
        MakeMoves();
        EndTurn();
    }
}

void Bot::InitializeTasks()
{
    /*
    for (auto &antPair : State.AllyAnts)
    {
        State.Bug << "Has Task (Initialize)? " << antPair.second->HasTask() << endl;
    }
    */

    // Ant Hill Protection ======================================================
    int antCount = State.AllyAnts.size();

    // Clear Wall
    _guardHillTasks.clear();
    
    int wallRange = (1*antCount/4) / (4*State.MyHills.size());

    if (wallRange >= 1)
    {
        // Create tasks for wall and retrieve possible candidates for it
        vector<Ant*> wallCandidateAnts;
        auto onVisited = [&,this](const Location &location, const int distance)
        {
            Square &visitedSquare = State.Grid[location.Row][location.Col];
            if (!visitedSquare.IsWater)
            {
                if (distance == wallRange)
                {
                    _guardHillTasks.push_back(GuardHillTask(&State, location, &visitedSquare));
                }

                // State.Bug << "Cond1 : " << ((distance <= wallRange)?"true":"false") << "Cond2 : " << ((wallCandidateAnts.size() < _guardHillTasks.size())?"true":"false") << endl;
                if ((distance <= wallRange) || (wallCandidateAnts.size() < _guardHillTasks.size()))
                {
                    /*
                    State.Bug << "Enter " << location.Row << "," << location.Col << " " << (visitedSquare.Ant != nullptr);
                    if (visitedSquare.Ant != nullptr)
                    {
                        State.Bug << " " << (visitedSquare.Ant->Team == 0) << " " << (!visitedSquare.Ant->HasTask());
                    }
                    State.Bug << endl;
                    */

                    if ((visitedSquare.Ant != nullptr) && 
                        (visitedSquare.Ant->Team == 0) && 
                        (!visitedSquare.Ant->HasTask()))
                    {
                        wallCandidateAnts.push_back(visitedSquare.Ant);
                        // State.Bug << "New Candidate " << location.Row << "," << location.Col << endl;
                    }
                    return false;
                }
                else
                {
                    return true;
                }
            }
            return false;
        };
        
        State.MultiBreadthFirstSearchAll(State.MyHills, wallRange+2, onVisited, false);

        while (wallCandidateAnts.size() > _guardHillTasks.size())
        {
            wallCandidateAnts.pop_back();
        }

        // Roughly shuffle tasks to prevent huge parts of walls without ants
        for (int i = _guardHillTasks.size() - 1; i > 0; --i)
        {
            swap(_guardHillTasks[i], _guardHillTasks[_guardHillTasks.size() - 1 - (rand() % (i + 1))]);
        }

        // Assign ants to tasks
        int j = 0;
        for (auto &task : _guardHillTasks)
        {
            // State.Bug << "ASSIGN TASK " << j++ << " Candidate Count : " << wallCandidateAnts.size() << endl;
            for (auto wallCandidate : wallCandidateAnts)
            {
                task.AddCandidate(wallCandidate);
            }
            
            // State.Bug << "Select Candidate" << endl;
            task.SelectCandidate();
            task.ClearCandidates();
        }
        
        /*
        for (auto &antPair : State.AllyAnts)
        {
            State.Bug << "Has Task? " << antPair.second->HasTask() << endl;
        }
        */
    }
}

void Bot::DoTasks()
{
    for (auto &antPair : State.AllyAnts)
    {
        if ((!antPair.second->Decided) &&
            (antPair.second->HasTask()))
        {
            antPair.second->CurrentTask->GiveOrderToAssignee();
        }
    }
}

void Bot::ClearFinishedTasks()
{
    // Clear invalid/finished guard hill tasks
    //State.Bug << "Size Before " << _guardHillTasks.size() <<endl;

    for (int i = _guardHillTasks.size() - 1; i >= 0; i--)
    {
        //State.Bug << "Hill Task " << endl;
        //State.Bug << "Valid? " << _guardHillTasks[i].IsValid() << " Completed?" << _guardHillTasks[i].IsCompleted() << endl;
        if ((!_guardHillTasks[i].IsValid()) || _guardHillTasks[i].IsCompleted())
        {
            _guardHillTasks.erase(_guardHillTasks.begin() + i); 
        }
        else
        {
            State.Bug << "Valid and/or Not Completed " << endl;
        }
    }

    /*
    for (auto &antPair : State.AllyAnts)
    {
        State.Bug << "Has Task (In ClearFinishedTask) ? " << antPair.second->HasTask() << endl;
    }

    State.Bug << "Size After " << _guardHillTasks.size() <<endl;
    */
}

// Makes the bots moves for the Turn
void Bot::MakeMoves()
{

    State.Bug << "Turn " << State.Turn << ":" << endl;
    // State.Bug << State << endl;

    State.Bug << "InitializeTasks" << endl;
    InitializeTasks();

    State.Bug << "Combat" << endl;
    Combat();

    State.Bug << "DestroyOtherHills" << endl; 
    DestroyOtherHills();

    State.Bug << "SeekFood" << endl;
    SeekFood();

    State.Bug << "Do Tasks" << endl;
    DoTasks();

    State.Bug << "ExploreFog" << endl;
    ExploreFog();

    State.Bug << "ClearFinishedTasks" << endl;
    ClearFinishedTasks();

    State.Bug << "FinalMove" << endl;
    int offset;
    int direction;

    Ant *ant;
    // Picks out a default move for each ant
    for (const auto &antPair : State.AllyAnts)
    {
        ant = antPair.second;
        // Check if ant already has decided on a move
        if (!ant->Decided)
        {
            offset = rand();
            for (int d = 0; d < TDIRECTIONS; d++)
            {
                direction = ( d + offset ) % TDIRECTIONS;
                Location destination = State.GetLocation(ant->CurrentLocation, direction);

                if ((!State.Grid[destination.Row][destination.Col].IsWater) &&
                    //(State.Grid[destination.Row][destination.Col].Ant == nullptr) &&
                    (State.Grid[destination.Row][destination.Col].HillPlayer != 0))
                {
                    ant->SetMoveDirection(direction);
                    break;
                }
            }
        }
    }

    // Make every ant move
    for (const auto &antPair : State.AllyAnts)
    {
        ant = antPair.second;
        MakeMove(ant);
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
        State.Grid[antLocation.Row][antLocation.Col].Ant->SetMoveDirection(direction);
    }
}

void Bot::SeekFood()
{
    for (const Location &foodLoc : State.Food)
    {
        MoveClosestAvailableAntTowards(foodLoc, (int)State.ViewRadius);
    }
}

void Bot::DestroyOtherHills()
{
    for (const Location &hillLoc : State.EnemyHills)
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

    // Compute life/death state for all opposing armies
    for (int i = 0; i < allyGroups.size(); i++)
    {
        unordered_set<Location, Location> movedAllyGroup = allyGroups[i];
        unordered_set<Location, Location> movedEnemyGroup = enemyGroups[i];

        // Initialize opponent count for allies and enemies
        for (const auto &ally : allyGroups[i]) 
        {
            State.Grid[ally.Row][ally.Col].Ant->SurroundingOpponentCount = 0;
        }
        for (const auto &enemy : enemyGroups[i]) 
        {
            State.Grid[enemy.Row][enemy.Col].Ant->SurroundingOpponentCount = 0;
        }

        // Compute opponent count for allies and enemies
        for (const auto &ally : allyGroups[i]) 
        {
            for (const auto &enemy : enemyGroups[i]) 
            {
                if (State.Distance2(ally, enemy) > State.AttackRadius2)
                    continue;
                
                State.Grid[ally.Row][ally.Col].Ant->SurroundingOpponentCount++;
                State.Grid[enemy.Row][enemy.Col].Ant->SurroundingOpponentCount++;
            }
        }

        // Compute how many ally ant will die
        int allyDeathCount = 0;
        for (const auto &ally : allyGroups[i]) 
        {
            for (const auto &enemy : enemyGroups[i]) 
            {
                if (State.Distance2(ally, enemy) > State.AttackRadius2)
                    continue;
                
                if (State.Grid[ally.Row][ally.Col].Ant->SurroundingOpponentCount >= State.Grid[enemy.Row][enemy.Col].Ant->SurroundingOpponentCount)
                {
                    allyDeathCount++;
                    break;
                }
            }
        }

        // Compute how many enemy ant will die
        int enemyDeathCount = 0;
        for (const auto &enemy : enemyGroups[i]) 
        {
            for (const auto &ally : allyGroups[i]) 
            {
                if (State.Distance2(enemy, ally) > State.AttackRadius2)
                    continue;
                
                if (State.Grid[enemy.Row][enemy.Col].Ant->SurroundingOpponentCount >= State.Grid[ally.Row][ally.Col].Ant->SurroundingOpponentCount)
                {
                    enemyDeathCount++;
                    break;
                }
            }
        }
    }
}

// Finishes the Turn
void Bot::EndTurn()
{
    if (State.Turn > 0)
    {
        State.Reset();
        _antsBlockedByOtherAnts.clear();
    }
    State.Turn++;

    cout << "go" << endl;
}


void Bot::ExploreFog()
{
    int direction;
    Location destination;
    
    Ant* ant;
    // Picks out moves for each ant
    for (const auto &antPair : State.AllyAnts)
    {
        ant = antPair.second;
        // Check if ant already moved
        if (!ant->Decided)
        {
            State.Bug << "Enter SearchMostFogged" << endl;

            destination = State.SearchMostFogged(ant->CurrentLocation, &direction, ((int)State.ViewRadius)+5);
            if (destination != Location(-1,-1))
            {
                ant->SetMoveDirection(direction);
            }

            State.Bug << "Exit SearchMostFogged" << endl;
        }
        else
        {
            State.Bug << "Ant (" << ant->Id<< ") at " << ant->CurrentLocation.Row <<
                "/" << ant->CurrentLocation.Col << "already decided" << endl;
        }
    }
}

// outputs move information to the engine
// and registers move info in ant
void Bot::MakeMove(Ant* ant)
{
    // State.Bug << "MAKE MOVE ant of ID " << ant->Id << endl;
    if (ant->MoveDirection == -1)
    {
        ant->NextLocation = ant->CurrentLocation;
        return;
    }

    Location nLoc = State.GetLocation(ant->CurrentLocation, ant->MoveDirection);

    Square &nextSquare = State.Grid[nLoc.Row][nLoc.Col];
    if (nextSquare.IsFood)
    {
        // Do not move if going towards food (food has collisions)
        ant->NextLocation = ant->CurrentLocation;
    }
    else if ((nextSquare.Ant != nullptr) && (nextSquare.Ant->Team == 0))
    {
        _antsBlockedByOtherAnts[nextSquare.Ant->Id] = ant;
        ant->NextLocation = ant->CurrentLocation;
    }
    else
    {
        cout << "o " << ant->CurrentLocation.Row << " " << ant->CurrentLocation.Col << " " << CDIRECTIONS[ant->MoveDirection] << endl;
        // State.Bug << "Moving ant of ID " << ant->Id << " to " << nLoc.Row << "/" << nLoc.Col << endl;
        ant->NextLocation = nLoc;
        
        State.Grid[ant->CurrentLocation.Row][ant->CurrentLocation.Col].Ant = nullptr;
        State.Grid[nLoc.Row][nLoc.Col].Ant = ant;

        auto blockedAntIterator = _antsBlockedByOtherAnts.find(ant->Id); 
        if (blockedAntIterator != _antsBlockedByOtherAnts.end())
        {
            MakeMove(blockedAntIterator->second);
        }
    }

    // TODO : check "Ant loops" in its own function 
}
