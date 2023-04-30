#include "Bot.h"

#include <unordered_set>
#include <algorithm>
#include <cstdlib>

#include "CombatState.h"

using namespace std;

// Constructor
Bot::Bot()
    : State()
    , CombatEvaluator(&State)
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
        State.UpdateHillInformation();
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

    InitializeGuardHillTasks();

    /*
    for (auto &antPair : State.AllyAnts)
    {
        State.Bug << "Has Task? " << antPair.second->HasTask() << endl;
    }
    */
}

void Bot::InitializeGuardHillTasks()
{
    // Clear previous guard tasks in order to update them
    _guardHillTasks.clear();
    _defendHillTasks.clear();
    _hillInvaderAnts.clear();

    // If no hill is still there, or not enough ants, no need to guard them
    if (State.MyHills.size() <= 0 || State.AllyAnts.size() < State.MyHills.size()*2)
        return;

    // Compute wall size
    int antCount = State.AllyAnts.size();
    int wallRange = (3*antCount/5) / (4*State.MyHills.size());
    int defenseRange = max(5, wallRange);

    // Create tasks for wall and retrieve possible candidates for it
    vector<Ant*> wallCandidateAnts;

    auto onVisited = [&,this](const Location &location, const int distance)
    {
        Square &visitedSquare = State.Grid[location.Row][location.Col];
        if (!visitedSquare.IsWater)
        {
            if (distance == wallRange && wallRange > 0)
            {
                _guardHillTasks.push_back(GuardHillTask(&State, location));
            }

            // State.Bug << "Cond1 : " << ((distance <= wallRange)?"true":"false") << "Cond2 : " << ((wallCandidateAnts.size() < _guardHillTasks.size())?"true":"false") << endl;
            if ((distance <= defenseRange) || (wallCandidateAnts.size() < _guardHillTasks.size()))
            {
                /*
                State.Bug << "Enter " << location.Row << "," << location.Col << " " << (visitedSquare.Ant != nullptr);
                if (visitedSquare.Ant != nullptr)
                {
                    State.Bug << " " << (visitedSquare.Ant->Team == 0) << " " << (!visitedSquare.Ant->HasTask());
                }
                State.Bug << endl;
                */

                if (visitedSquare.Ant != nullptr) 
                {
                    if ((visitedSquare.Ant->Team == 0) && 
                        (!visitedSquare.Ant->HasTask()))
                    {
                        wallCandidateAnts.push_back(visitedSquare.Ant);
                        // State.Bug << "New Candidate " << location.Row << "," << location.Col << endl;
                    }
                    else
                    {
                        if(visitedSquare.Ant->Team > 0)
                            _hillInvaderAnts.push_back(Location(location));
                    }
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
    State.MultiBreadthFirstSearchAll(vector(State.MyHills.begin(), State.MyHills.end()), defenseRange+2, onVisited, false);

    for (auto& invaderLocation : _hillInvaderAnts)
    {
        Location closestHill;
        int closestDist = State.Rows+State.Cols;

        for(auto& hill : State.MyHills)
        {
            if(State.ManhattanDistance(invaderLocation, hill) < closestDist)
            {
                closestHill = hill;
                closestDist = State.ManhattanDistance(invaderLocation, hill);
            }
        }

        Location midPoint = State.GetMiddlePoint(closestHill, invaderLocation);
        if (!State.Grid[midPoint.Row][midPoint.Col].IsWater)
            _defendHillTasks.push_back(ReachLocationTask(&State, invaderLocation));
    }

    
    for (auto &task : _defendHillTasks)
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
    
    // Get rid of excess candidates
    
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
}

void Bot::InitializeAllyReinforcementTasks()
{
    
}

void Bot::DoTasks()
{
    for (auto &antPair : State.AllyAnts)
    {
        if ((!antPair.second->Decided) &&
            (antPair.second->HasTask()) &&
            (antPair.second->CurrentTask->IsValid()))
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

    State.Bug << "Compute Armies" << endl;
    ComputeArmies();
    
    State.Bug << "ClearFinishedTasks" << endl;
    ClearFinishedTasks();

    State.Bug << "InitializeTasks" << endl;
    InitializeTasks();

    for (auto &task : _defendHillTasks)
    {
        if(task.IsValid() && task.IsAssigned())
            task.GiveOrderToAssignee();
    }

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

    State.Bug << "FinalMove End" << endl;

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
        MoveClosestAvailableAntTowards(foodLoc, (int)(1.25 * State.ViewRadius));
    }
}

void Bot::DestroyOtherHills()
{
    for (const Location &hillLoc : State.EnemyHills)
    {
        MoveClosestAvailableAntTowards(hillLoc, (int)(2 * State.ViewRadius));
    }
}

void Bot::DefendHills()
{

}

void Bot::Combat()
{
    // Compute combat move for all armies
    for (int i = 0; i < allyGroups.size(); i++)
    {
        State.Bug << "Combat " << i << endl;
        State.Bug << "   " << "A=" << allyGroups[i].size() << endl;
        for (const Location &ally : allyGroups[i])
        {
            State.Bug << "      " << ally.Row << "/" << ally.Col << endl;
        }
        State.Bug << "   " << "E=" << enemyGroups[i].size() << endl;
        for (const Location &enemy : enemyGroups[i])
        {
            State.Bug << "      " << enemy.Row << "/" << enemy.Col << endl;
        }

        CombatState combatState;
        for (const auto &allyLoc : allyGroups[i]) 
        {
            combatState.UnmovedAllies.push(State.Grid[allyLoc.Row][allyLoc.Col].Ant);
        }
        
        for (const auto &enemyLoc : enemyGroups[i]) 
        {
            combatState.UnmovedEnemies.push(State.Grid[enemyLoc.Row][enemyLoc.Col].Ant);
        }
        CombatEvaluator.ComputeBestMove(&combatState);

        while (!CombatEvaluator.BestMoves.empty())
        {
            pair<Ant*, int> &bestMove = CombatEvaluator.BestMoves.top();
            if (!State.AllyAnts[bestMove.first->Id]->Decided)
                State.AllyAnts[bestMove.first->Id]->SetMoveDirection(bestMove.second);
            CombatEvaluator.BestMoves.pop();
        }
    }
}

void Bot::ComputeArmies()
{
    // (Re)initialize ally/enemy groups
    allyGroups.clear();
    enemyGroups.clear();

    if (State.EnemyAnts.empty())
        return;

    // Search for enemy groups close to allies
    //   Initialization
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
    //  Begin search
    for (const Location &antLoc : State.EnemyAnts)
    {
        allyGroup.clear();
        State.CircularBreadthFirstSearchAll(antLoc, (State.AttackRadius + 2.01) * (State.AttackRadius + 2.01), onVisited, true);
        
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
        // State.Bug << "Food Blocked Ant" << endl;
        // Do not move if going towards food (food has collisions)
        ant->NextLocation = ant->CurrentLocation;
    }
    else if (nextSquare.Ant != nullptr)
    {
        if (nextSquare.Ant->Team == 0)
        {
            // State.Bug << "Ally Blocked Ant" << endl;
            _antsBlockedByOtherAnts[nextSquare.Ant->Id] = ant;
        }
        // else
        // {
        //     State.Bug << "Enemy Blocked Ant" << endl;
        // }

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
            // State.Bug << "Unlocked Ant";
            MakeMove(blockedAntIterator->second);
        }
    }

    // TODO : check "Ant loops" in its own function 
}
