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

// Creates and assign tasks to ants
void Bot::InitializeTasks()
{
    /*
    for (auto &antPair : State.AllyAnts)
    {
        State.Bug << "Has Task (Initialize)? " << antPair.second->HasTask() << endl;
    }
    */

    InitializeGuardHillTasks();
    // InitializeAllyReinforcementTasks();

    /*
    for (auto &antPair : State.AllyAnts)
    {
        State.Bug << "Has Task? " << antPair.second->HasTask() << endl;
    }
    */
}

// Creates and assign tasks related to guarding and defending the hill
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
    int wallRange;

    if (antCount > (80 * State.MyHills.size()))
    {
        wallRange = (antCount - 25) / (4*State.MyHills.size());
    }
    else 
    {
        wallRange = (1*antCount/4) / (4*State.MyHills.size());
    }

    int defenseRange = max(3, wallRange);

    // Create tasks for wall and retrieve possible candidates for it
    vector<Ant*> wallCandidateAnts;

    auto onVisited = [&,this](const Location &location, const int distance, const int outDirection)
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
    State.MultiBreadthFirstSearchAll(vector(State.MyHills.begin(), State.MyHills.end()), defenseRange+10, onVisited, false);

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

    // Roughly shuffle tasks to prevent huge parts of walls without ants
    for (int i = _guardHillTasks.size() - 1; i > 0; --i)
    {
        swap(_guardHillTasks[i], _guardHillTasks[_guardHillTasks.size() - 1 - (rand() % (i + 1))]);
    }

    // Get rid of some tasks to allow for free move space inside wall
    if (wallRange > 2)
    {
        int removedWallTasks = (_guardHillTasks.size() * 1) / 16;
        if (removedWallTasks > 4)
            removedWallTasks = 4;

        for (int i = 0; i < removedWallTasks; i++)
        {
            _guardHillTasks.pop_back();
        }
    }
    
    // Get rid of excess candidates
    while (wallCandidateAnts.size() > _guardHillTasks.size())
    {
        wallCandidateAnts.pop_back();
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

// Creates and assign All
// TODO commenter si on l'utilise ou pas
void Bot::InitializeAllyReinforcementTasks()
{
    // Create tasks if needed
    // State.Bug << "CREATE REINFORCEMENTS" << endl;
    if (State.AllyAnts.size() > 18 * State.MyHills.size())
    {
        const int antsPerReinforcement = 1;
        for (int i = 0; i < allyGroups.size(); i++)
        {
            if (allyGroups[i].size() > enemyGroups[i].size())
                continue;

            // Create "antsPerReinforcement" tasks per ant in a "dangerous" encouter
            // State.Bug << "   Create Reinforcement for Group " << i << endl;
            for (const Location &allyLoc : allyGroups[i])
            {
                int allyId = State.Grid[allyLoc.Row][allyLoc.Col].Ant->Id;
                if (_allyReinforcementTasks.count(allyId) <= 0)
                {
                    // State.Bug << "     Create Reinforcement for Ant at : " << allyLoc.Col << ":" << allyLoc.Row << endl;
                    vector<ReachAntTask*> reinforcementTasks;
                    for (int j = 0; j < antsPerReinforcement; j++)
                    {
                        // State.Bug << "         Create single Reinforcement" << endl;
                        reinforcementTasks.push_back(new ReachAntTask(&State, allyId, 2));
                    }
                    // State.Bug << "     Add reinforcements" << endl;
                    _allyReinforcementTasks[allyId] = reinforcementTasks;
                }
            }
        }
    }
    

    // Assign tasks
    if (_allyReinforcementTasks.size() <= 0)
        return;

    // State.Bug << "ASSIGN REINFORCEMENTS" << endl;
    vector<Ant*> availableAnts;
    for (const auto &antPair : State.AllyAnts)
    {
        if (!antPair.second->HasTask())
            availableAnts.push_back(antPair.second);
    }

    auto antsIt = State.AllyAnts.begin();
    auto antsItEnd = State.AllyAnts.end();

    for (auto &reinforcementTasksPair : _allyReinforcementTasks)
    {
        for (auto &reinforcementTask : reinforcementTasksPair.second)
        {
            if (reinforcementTask->IsAssigned())
                continue;
            
            // Assign first best available ant to task
            bool candidateFound = false;
            for (const Ant* ant : availableAnts)
            {
                if (!ant->HasTask() && (State.ManhattanDistance(ant->CurrentLocation, State.AllyAnts[reinforcementTasksPair.first]->CurrentLocation) > 5))
                {
                    reinforcementTask->AddCandidate(antsIt->second);
                    candidateFound = true;
                }
            }

            if (candidateFound)
            {
                reinforcementTask->SelectCandidate();
                reinforcementTask->ClearCandidates();
            }

            /*
            while ((antsIt != antsItEnd) && (!reinforcementTask->IsAssigned()))
            {
                if ((antsIt->first != reinforcementTasksPair.first) && (!antsIt->second->HasTask()))
                {
                    reinforcementTask->AddCandidate(antsIt->second);
                    reinforcementTask->SelectCandidate();
                    reinforcementTask->ClearCandidates();
                    // State.Bug << "   Assign Reinforcement with Ant at : " << antsIt->second->CurrentLocation.Col << ":" << antsIt->second->CurrentLocation.Row << endl;
                }
                ++antsIt;
            }
            */
        }
    }
}

// Makes Ants perform tasks
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

// Remove all tasks that are either completed or irrelevant
void Bot::ClearFinishedTasks()
{
    // Clear invalid/finished guard hill tasks
    //State.Bug << "Size Before " << _guardHillTasks.size() <<endl;
    
    // Clear finished guard hill tasks
    for (int i = _guardHillTasks.size() - 1; i >= 0; i--)
    {
        //State.Bug << "Hill Task " << endl;
        //State.Bug << "Valid? " << _guardHillTasks[i].IsValid() << " Completed?" << _guardHillTasks[i].IsCompleted() << endl;
        if ((!_guardHillTasks[i].IsValid()) || _guardHillTasks[i].IsCompleted())
        {
            _guardHillTasks.erase(_guardHillTasks.begin() + i); 
        }
        // else
        // {
        //     State.Bug << "Valid and/or Not Completed " << endl;
        // }
    }

    // Clear finished ally reinforcement tasks
    unordered_set<int> fullyFinishedReinforcementTasks;
    for (auto &reinforcementTasksPair : _allyReinforcementTasks)
    {
        vector<ReachAntTask*> &reinforcementTasks = reinforcementTasksPair.second;

        for (int i = reinforcementTasks.size() - 1; i >= 0; i--)
        {
            // State.Bug << " REINFORCE TASK STATE i=" << i << " " << 
            //     reinforcementTasks[i]->IsValid() << " " << 
            //     reinforcementTasks[i]->IsCompleted() << " " <<
            //     reinforcementTasks[i]->IsAssigned() << endl;
            if ((!reinforcementTasks[i]->IsValid()) || reinforcementTasks[i]->IsCompleted())
            {
                // State.Bug << " ERASE REINFORCE TASK" << endl;
                ReachAntTask* toErase = reinforcementTasks[i];
                reinforcementTasks.erase(reinforcementTasks.begin() + i);
                delete toErase;
            }
        }

        if (reinforcementTasks.size() == 0)
        {
            fullyFinishedReinforcementTasks.insert(reinforcementTasksPair.first);
        }
    }

    for (int fullyFinishedTask : fullyFinishedReinforcementTasks)
    {
        _allyReinforcementTasks.erase(fullyFinishedTask);
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

    State.Bug << "DefendHills" << endl;
    DefendHills();

    State.Bug << "SeekFood" << endl;
    SeekFood();

    State.Bug << "Combat" << endl;
    Combat();

    State.Bug << "DestroyOtherHills" << endl; 
    DestroyOtherHills();

    State.Bug << "Do Tasks" << endl;
    DoTasks();

    State.Bug << "ApproachEnemies" << endl;
    ApproachEnemies();

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

// Orders ants to look for and go towards nearby food
void Bot::SeekFood()
{
    unordered_map<Location, vector<Location>, Location> antsToFoods;
    unordered_map<Location, vector<int>, Location> antDirections; 
    for (const Location &foodLoc : State.Food)
    {
         State.Bug << "For Food "<< foodLoc.Row<< "/"<<foodLoc.Col << endl;
        int direction;
        Location antLocation;

        antLocation = State.BreadthFirstSearch(
            foodLoc,
            &direction, 
            1.25 * State.ViewRadius,
            [this](const Location& location)
            {
                return State.IsAvailableAnt(location);
            }
        );

        if (!(antLocation == Location(-1,-1)))
        {
            antsToFoods[antLocation].push_back(foodLoc);
            antDirections[antLocation].push_back(direction);
        }
    }

    for(auto& antFoodPair : antsToFoods)
    {
        int bestDist = State.Rows+State.Cols;
        int bestDirection = -1;
        int dist;
        State.Bug << "Food Ant at "<< antFoodPair.first.Row<< "/"<<antFoodPair.first.Col << endl;
        for(int i = 0; i < antFoodPair.second.size(); i++)
        {
            dist = State.ManhattanDistance(antFoodPair.first, antFoodPair.second[i]);
            if(dist < bestDist)
            {
                State.Bug << "Food at "<< antFoodPair.second[i].Row<< "/"<<antFoodPair.second[i].Col << "is best" << endl;
                bestDist = dist;
                bestDirection = antDirections[antFoodPair.first][i];
            }
        }
        State.Bug << "Best Direction is " << (bestDirection != -1 ? CDIRECTIONS[bestDirection] : 'O') << endl;
        State.Grid[antFoodPair.first.Row][antFoodPair.first.Col].Ant->SetMoveDirection(bestDirection);
    }
}

// Orders ants go to hills to destroy them
void Bot::DestroyOtherHills()
{
    for (const Location &hillLoc : State.EnemyHills)
    {
        MoveClosestAvailableAntsTowards(hillLoc, (int)(2 * State.ViewRadius));
    }
}

// Sends ants to protect an ally hill from approaching enemies
void Bot::DefendHills()
{
    for (auto &task : _defendHillTasks)
    {
        if(task.IsValid() && task.IsAssigned())
            task.GiveOrderToAssignee();
    }
}

// Makes groups of allies formed by ComputeArmies() move in order to resolve battles
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

// Identifies occuring battles and groups the allies and enemies of each battle 
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
        if (State.IsAvailableAnt(location))
            allyGroup.insert(location);
    };
    //  Begin search
    for (const Location &antLoc : State.EnemyAnts)
    {
        allyGroup.clear();
        State.CircularBreadthFirstSearchAll(antLoc, (State.AttackRadius + 1.75) * (State.AttackRadius + 1.75), onVisited, false);
        
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

// Orders ants to go to places that are in the Fog Of War
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

// Orders ants to follow nearby enemies
void Bot::ApproachEnemies()
{
    for (const Location &enemy : State.EnemyAnts)
    {
        MoveClosestAvailableAntsTowards(enemy, State.AttackRadius+4, 3);
    }
}

// Orders maxAnts ants in searchRadius to move one step towards targetLocation
// This general function is used by other functions
void Bot::MoveClosestAvailableAntsTowards(const Location &targetLocation, int searchRadius, int maxAnts)
{
    int movedAnts = 0;
    State.MultiBreadthFirstSearchAll(
        vector<Location>(1,targetLocation),
        searchRadius,
        [&](const Location& location, int distance, int directionToTake)
        {
            if (State.IsAvailableAnt(location))
            {
                State.Grid[location.Row][location.Col].Ant->SetMoveDirection(directionToTake);
                movedAnts++;
                return (movedAnts >= maxAnts);
            }
            return false;
        }
    );
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
            auto blockedAntIterator = _antsBlockedByOtherAnts.find(ant->Id);
            if ((blockedAntIterator != _antsBlockedByOtherAnts.end()) && (blockedAntIterator->second->Id == nextSquare.Ant->Id))
            {
                State.Bug << "Swap Ants " << nLoc.Row << ":" << nLoc.Col << " VS " << ant->CurrentLocation.Row << ":" << ant->CurrentLocation.Col << endl;

                Ant* otherAnt = State.Grid[nLoc.Row][nLoc.Col].Ant;
                cout << "o " << ant->CurrentLocation.Row << " " << ant->CurrentLocation.Col << " " << CDIRECTIONS[ant->MoveDirection] << endl;
                cout << "o " << otherAnt->CurrentLocation.Row << " " << otherAnt->CurrentLocation.Col << " " << CDIRECTIONS[otherAnt->MoveDirection] << endl;
                
                State.Grid[nLoc.Row][nLoc.Col].Ant = ant;
                State.Grid[ant->CurrentLocation.Row][ant->CurrentLocation.Col].Ant = otherAnt;

                ant->NextLocation = blockedAntIterator->second->CurrentLocation;
                blockedAntIterator->second->NextLocation = ant->CurrentLocation;
            }
            else
            {
                _antsBlockedByOtherAnts[nextSquare.Ant->Id] = ant;
                ant->NextLocation = ant->CurrentLocation;
            }
        }
        else
        {
            ant->NextLocation = ant->CurrentLocation;
        }
        // else
        // {
        //     State.Bug << "Enemy Blocked Ant" << endl;
        // }
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
}
