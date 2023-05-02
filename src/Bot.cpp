#include "Bot.h"

#include <unordered_set>
#include <algorithm>
#include <cstdlib>

#include "CombatState.h"
#include "WrapGridAlgorithm.h"

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
    WrapGridAlgorithm::InitializeSize(State.Rows, State.Cols);
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

    State.Bug << "MakeDefaultMove" << endl;
    MakeDefaultMove();

    State.Bug << "ExecuteMoves" << endl;
    ExecuteMoves();

    State.Bug << "time taken: " << State.Timer.GetTime() << "ms" << endl << endl;
}

// Finishes the Turn
void Bot::EndTurn()
{
    // Reset state data
    if (State.Turn > 0)
    {
        State.Reset();
        _antsBlockedByOtherAnts.clear();
    }
    // Increment turn count
    State.Turn++;
    
    // Tell the engine that the bot has finished its turn
    cout << "go" << endl;
}

// Orders maxAnts ants in searchRadius to move one step towards targetLocation
// This general function is used by other functions
void Bot::MoveClosestAvailableAntsTowards(const Location &targetLocation_r, int searchRange, int maxAnts)
{
    int movedAnts = 0;

    // Do a BFS starting from the target in order to find
    // candidates within 'searchRange' for moving toward target
    WrapGridAlgorithm::BreadthFirstSearchSingle
    (
        targetLocation_r,
        searchRange,
        [&](const Location &loc_r) { return !State.Grid[loc_r.Row][loc_r.Col].IsWater; },
        [&](const Location& location, int distance, int directionTowardTarget)
        {
            if (State.IsAvailableAnt(location))
            {
                State.Grid[location.Row][location.Col].Ant_p->SetMoveDirection(directionTowardTarget);
                movedAnts++;
                return (movedAnts >= maxAnts);
            }
            return false;
        }
    );
}

// Orders ants to look for and go towards nearby food
void Bot::SeekFood()
{
    // finding food is done the other way around : foods find their nearest (ally) ant
    
    // both these maps are used to store an ant and information about foods where 'ant' is the food's nearest ant
    // map of <ant, vector of distances>
    // the vector part contains distances from 'ant' to foods that are nearest
    unordered_map<Location, vector<int>, Location> antsDistanceToFoods;
    // map of <ant, vector of directions>
    // the vector part contains all first directions to take from 'ant' to the nearest foods
    unordered_map<Location, vector<int>, Location> antDirections; 
    
    for (const Location &foodLoc_r : State.Food)
    {
        bool antFound = false;
        int direction;
        Location antLocation;
        int antDistance;

        // Look for single nearest ally ant starting from foodLoc
        WrapGridAlgorithm::BreadthFirstSearchSingle(
            foodLoc_r,
            1.25 * State.ViewRadius,
            [&](const Location &loc_r) { return !State.Grid[loc_r.Row][loc_r.Col].IsWater; },
            [&](const Location &location_r, int distance, int directionTowardFood)
            {
                if (State.IsAvailableAnt(location_r))
                {
                    antLocation = location_r;
                    direction = directionTowardFood;
                    antFound = true;
                    antDistance = distance;
                    return true;
                }
                return false;
            }
        );
        
        // register ant info if found
        if (antFound)
        {
            antsDistanceToFoods[antLocation].push_back(antDistance);
            antDirections[antLocation].push_back(direction);
        }
    }

    // make ants go towards the food that's nearest to them
    for (auto &antDistancePair_r : antsDistanceToFoods)
    {
        int bestDist = State.Rows+State.Cols; // Put a very high value by default
        int bestDirection = -1; // default direction is neutral
        int dist;
        
        // iterate and find best distance and direction towards foods for current antonVisited_f_f
        for (int i = 0; i < antDistancePair_r.second.size(); i++)
        {
            dist = antDistancePair_r.second[i];
            if (dist < bestDist)
            {
                bestDist = dist;
                bestDirection = antDirections[antDistancePair_r.first][i];
            }
        }
        // perform move
        State.Grid[antDistancePair_r.first.Row][antDistancePair_r.first.Col].Ant_p->SetMoveDirection(bestDirection);
    }
}

// Orders ants to go to places that are in the Fog Of War
void Bot::ExploreFog()
{
    // Initialize all exploration
    int currentScore;
    auto isNotWaterPredicate_f = [&](const Location &loc_r) { return !State.Grid[loc_r.Row][loc_r.Col].IsWater; };
    auto onVisited_f = [&](const Location &location_r, int distance, int direction)
    {
        currentScore += State.Grid[location_r.Row][location_r.Col].TurnsInFog;
        return false;
    };

    // Search for available ants
    for (const auto &antPair_r : State.AllyAnts)
    {
        Ant *ant_p = antPair_r.second;

        // If ant has already chosen a direction, don't use it for exploration
        if (ant_p->Decided)
            continue;

        // Initialize ant's exploration
        int bestDirection = -1;
        int bestScore = -1;
        // Search for best exploration direction
        for (int exploreDir = 0; exploreDir < TDIRECTIONS; exploreDir++)
        {
            Location explorationLoc = WrapGridAlgorithm::GetLocation(ant_p->CurrentLocation, exploreDir);

            // If exploration direction is on water or another ant, don't check it
            if ((State.Grid[explorationLoc.Row][explorationLoc.Col].Ant_p != nullptr) || 
                (State.Grid[explorationLoc.Row][explorationLoc.Col].IsWater))
            {
                continue;
            }

            // Initialize exploration in direction
            currentScore = 0;
            WrapGridAlgorithm::BreadthFirstSearchSingle(
                explorationLoc,
                ((int)State.ViewRadius)+5,
                isNotWaterPredicate_f,
                onVisited_f
            );

            // If a better direction was found, register it
            if ((currentScore > 0) && (currentScore >= bestScore))
            {
                bestDirection = exploreDir;
                bestScore = currentScore;
            }
        }

        if (bestDirection != -1)
        {
            ant_p->SetMoveDirection(bestDirection);
        }
    }
}

// Orders ants to go to hills to destroy them
void Bot::DestroyOtherHills()
{
    for (const Location &hillLoc_r : State.EnemyHills)
    {
        MoveClosestAvailableAntsTowards(hillLoc_r, (int)(2 * State.ViewRadius));
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

    auto isNotWaterPredicate_f = [&](const Location &loc_r) { return !State.Grid[loc_r.Row][loc_r.Col].IsWater; };
    auto onVisited_f = [&](const Location &location_r, int distance, int direction)
    {
        if (State.IsAvailableAnt(location_r))
            allyGroup.insert(location_r);
        return false;
    };

    //  Begin search
    for (const Location &antLoc_r : State.EnemyAnts)
    {
        // Create an ally group around an enemy
        allyGroup.clear();
        WrapGridAlgorithm::CircularBreadthFirstSearch(
            antLoc_r, 
            (State.AttackRadius + 2.01) * (State.AttackRadius + 2.01),
            isNotWaterPredicate_f, 
            onVisited_f
        );

        // If any allies are inside the group, add the group to the vector
        // of ally groups, and add the corresponding single enemy to an enemy group
        if (allyGroup.size() > 0)
        {
            unordered_set<Location, Location> allyGroupCopy = allyGroup;
            allyGroups.push_back(allyGroupCopy);

            unordered_set<Location, Location> enemyGroup;
            enemyGroup.insert(antLoc_r);
            enemyGroups.push_back(enemyGroup);
        }
    }

    // Merge groups that are close to each-other to create "armies".
    // Groups are considered close to each-other if they contain the same ally.
    for (int i = 0; i < allyGroups.size(); i++)
    {
        bool merge = false;
        for (int j = 0; j < allyGroups.size(); j++)
        {
            if (i == j)
                continue;
            
            // Check whether a merge should be made
            // (whether an ally is inside different groups)
            merge = false;
            for (const Location &allyLoc_r : allyGroups[i])
            {
                if (allyGroups[j].count(allyLoc_r) > 0)
                {
                    merge = true;
                    break;
                }
            }

            // Merge groups if needed
            if (merge)
            {
                // Merge ally groups
                allyGroups[i].insert(allyGroups[j].begin(), allyGroups[j].end());
                allyGroups.erase(allyGroups.begin() + j);
                // Merge their corresponding enemy groups
                enemyGroups[i].insert(enemyGroups[j].begin(), enemyGroups[j].end());
                enemyGroups.erase(enemyGroups.begin() + j);
                
                // Update loop indices according to merge position
                if (j < i)
                    i--;
                
                j--;
            }
        }
    }
}

// Makes groups of allies formed by ComputeArmies() move in order to resolve battles
void Bot::Combat()
{
    // Compute combat move for all armies
    for (int i = 0; i < allyGroups.size(); i++)
    {
        // State.Bug << "Combat " << i << endl;
        // State.Bug << "   " << "A=" << allyGroups[i].size() << endl;
        // for (const Location &ally_r : allyGroups[i])
        // {
        //     State.Bug << "      " << ally_r.Row << "/" << ally_r.Col << endl;
        // }
        // State.Bug << "   " << "E=" << enemyGroups[i].size() << endl;
        // for (const Location &enemy_r : enemyGroups[i])
        // {
        //     State.Bug << "      " << enemy_r.Row << "/" << enemy_r.Col << endl;
        // }

        // Initialize starting combat state for one ally army VS one enemy army
        CombatState combatState;
        for (const auto &allyLoc_r : allyGroups[i]) 
        {
            combatState.UnmovedAllies.push(State.Grid[allyLoc_r.Row][allyLoc_r.Col].Ant_p);
        }
        
        for (const auto &enemyLoc_r : enemyGroups[i]) 
        {
            combatState.UnmovedEnemies.push(State.Grid[enemyLoc_r.Row][enemyLoc_r.Col].Ant_p);
        }
        // Attempt to compute the best move for each ally ant 
        CombatEvaluator.ComputeBestMove(&combatState);
        
        // Set move for all ants inside ally army
        while (!CombatEvaluator.BestMoves.empty())
        {
            pair<Ant*, int> &bestMove_r = CombatEvaluator.BestMoves.top();
            if (!State.AllyAnts[bestMove_r.first->Id]->Decided)
                State.AllyAnts[bestMove_r.first->Id]->SetMoveDirection(bestMove_r.second);
            CombatEvaluator.BestMoves.pop();
        }
    }
}

// Orders ants to follow nearby enemies
void Bot::ApproachEnemies()
{
    for (const Location &enemy_r : State.EnemyAnts)
    {
        MoveClosestAvailableAntsTowards(enemy_r, State.AttackRadius+4, 3);
    }
}

// Orders ants to make their default move if they aren't doing anything else
void Bot::MakeDefaultMove()
{
    int offset;
    int direction;
    Ant *ant_p;
    // Picks out a default move for each ant (random available direction)
    for (const auto &antPair_r : State.AllyAnts)
    {
        ant_p = antPair_r.second;
        // Check if ant already has decided on a move
        if (!ant_p->Decided)
        {
            offset = rand();
            for (int d = 0; d < TDIRECTIONS; d++)
            {
                direction = ( d + offset ) % TDIRECTIONS;
                Location destination = WrapGridAlgorithm::GetLocation(ant_p->CurrentLocation, direction);

                if ((!State.Grid[destination.Row][destination.Col].IsWater) &&
                    (State.Grid[destination.Row][destination.Col].HillPlayer != 0)) // Prevent move on own hill
                {
                    ant_p->SetMoveDirection(direction);
                    break;
                }
            }
        }
    }
}

// Move every ant that has chosen a move
void Bot::ExecuteMoves()
{
    Ant *ant_p;
    // Execute move for every ant
    for (const auto &antPair_r : State.AllyAnts)
    {
        ant_p = antPair_r.second;
        MakeMove(ant_p);
    }
}

// Creates and assign tasks to ants
void Bot::InitializeTasks()
{
    /*
    for (auto &antPair_r : State.AllyAnts)
    {
        State.Bug << "Has Task (Initialize)? " << antPair_r.second->HasTask() << endl;
    }
    */

    InitializeHillProtectionTasks();

    /*
    for (auto &antPair_r : State.AllyAnts)
    {
        State.Bug << "Has Task? " << antPair_r.second->HasTask() << endl;
    }
    */
}

// Creates and assign tasks related to guarding and defending the hill
void Bot::InitializeHillProtectionTasks()
{
    // Clear previous guard tasks in order to update them
    // they are all cleared because information may have changed from one turn to the other
    _guardHillTasks.clear();
    _defendHillTasks.clear();
    _hillInvaderAnts.clear();

    // If no hill is still there, or not enough ants, no need to guard them
    if (State.MyHills.size() <= 0 || State.AllyAnts.size() < State.MyHills.size()*2)
        return;

    // Compute wall size
    int antCount = State.AllyAnts.size();

    int wallRange; // BFS range at which the wall will be made

    // Update wall range according to ant count
    // Starting from 70 ant (or 50 per hill if we have more than one hill),
    // only 20 are used to do something else than creating the wall.
    // This allows for a huge (and hopefully untraversable) wall to be made.
    if (((State.MyHills.size() == 1) && (antCount > 60)) || (antCount > (State.MyHills.size() * 40)))
    {
        wallRange = (antCount - 20) / (4*State.MyHills.size());
    }
    else
    {
        wallRange = (1*antCount/4) / (4*State.MyHills.size());
    }

    // Range at which enemy ants will have ally ants rushing towards them
    int defenseRange = max(4, wallRange);

    // Create tasks for wall and retrieve possible candidates for it
    vector<Ant*> hillProtectionCandidates;

    // Info processing function for when a tile is visited by BFS
    auto onVisited_f = [&,this](const Location &location_r, const int distance, const int outDirection)
    {
        Square &visitedSquare_r = State.Grid[location_r.Row][location_r.Col];
        if (!visitedSquare_r.IsWater)
        {
            // Create GuardHillTasks hill tasks at WallRange
            // This effectively creates walls around hills that are diamond-shaped 
            if (distance == wallRange && wallRange > 0)
            {
                _guardHillTasks.push_back(GuardHillTask(&State, location_r));
            }
            
            // Continue looking for ants when in the defenseRange or when there are not enough ants
            // to defend the hill
            if ((distance <= defenseRange) || (hillProtectionCandidates.size() < _guardHillTasks.size()))
            {
                // Check for Ant in square
                if (visitedSquare_r.Ant_p != nullptr) 
                {
                    // Add candidate for protecting the hill
                    if ((visitedSquare_r.Ant_p->Team == 0) && 
                        (!visitedSquare_r.Ant_p->HasTask()))
                    {
                        hillProtectionCandidates.push_back(visitedSquare_r.Ant_p);
                        // State.Bug << "New Candidate " << location.Row << "," << location.Col << endl;
                    }
                    else
                    {
                        // Detect enemy invaders (ants that are inside the wall or too close to the hill)
                        if (visitedSquare_r.Ant_p->Team > 0)
                            _hillInvaderAnts.push_back(Location(location_r));
                    }
                }
                return false;
            }
            else
            {
                // when there are enough ants found and the searching range is beyond the defenseRange, stop searching
                return true;
            }
        }
        return false;
    };

    WrapGridAlgorithm::BreadthFirstSearchMultiple(
        vector(State.MyHills.begin(), State.MyHills.end()),
        defenseRange+10,
        [&](const Location &loc_r) { return !State.Grid[loc_r.Row][loc_r.Col].IsWater; },
        onVisited_f
    );

    // Create DefendHillTasks
    for (auto &invaderLocation_r : _hillInvaderAnts)
    {
        _defendHillTasks.push_back(ReachLocationTask(&State, invaderLocation_r));
    }

    // State.Bug << "Defend Hill Tasks Created" << endl;
    
    // Assign defendHillTasks to fittest candidates
    for (auto &task_r : _defendHillTasks)
    {
        for (auto wallCandidate : hillProtectionCandidates)
        {
            task_r.AddCandidate(wallCandidate);
        }
        
        // State.Bug << "Select Candidate" << endl;
        task_r.SelectCandidate();
        task_r.ClearCandidates();
    }


    // Prepare GuardHillTasks (Forming a wall around the hills)

    // State.Bug << "Prepare GuardHillTasks" << endl;

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
    
    // Get rid of excess candidates (starting from the furthest ants from the hill)
    while (hillProtectionCandidates.size() > _guardHillTasks.size())
    {
        hillProtectionCandidates.pop_back();
    }

    // Assign ants to guardHillTasks
    int j = 0;
    for (auto &task_r : _guardHillTasks)
    {
        for (auto wallCandidate : hillProtectionCandidates)
        {
            task_r.AddCandidate(wallCandidate);
        }
        
        // State.Bug << "Select Candidate" << endl;
        task_r.SelectCandidate();
        task_r.ClearCandidates();
    }
}

// Makes Ants perform tasks
void Bot::DoTasks()
{
    for (auto &antPair_r : State.AllyAnts)
    {
        if ((!antPair_r.second->Decided) &&
            (antPair_r.second->HasTask()) &&
            (antPair_r.second->CurrentTask_p->IsValid()))
        {
            antPair_r.second->CurrentTask_p->GiveOrderToAssignee();
        }
    }
}

// Sends ants to protect an ally hill from approaching enemies
void Bot::DefendHills()
{
    for (auto &task_r : _defendHillTasks)
    {
        if (task_r.IsValid() && task_r.IsAssigned())
            task_r.GiveOrderToAssignee();
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

    /*
    for (auto &antPair_r : State.AllyAnts)
    {
        State.Bug << "Has Task (In ClearFinishedTask) ? " << antPair_r.second->HasTask() << endl;
    }

    State.Bug << "Size After " << _guardHillTasks.size() <<endl;
    */
}

// Outputs move information to the engine
// and registers move info in ant
void Bot::MakeMove(Ant *ant_p)
{
    // State.Bug << "MAKE MOVE ant of ID " << ant->Id << endl;

    // If ant has chosen to stay in place, don't move it
    if (ant_p->MoveDirection == -1)
    {
        ant_p->NextLocation = ant_p->CurrentLocation;
        return;
    }

    Location nLoc = WrapGridAlgorithm::GetLocation(ant_p->CurrentLocation, ant_p->MoveDirection);
    Square &nextSquare_r = State.Grid[nLoc.Row][nLoc.Col];

    if (nextSquare_r.IsFood) // Do not move if going towards food (food has collisions)
    {
        // State.Bug << "Food Blocked Ant" << endl;
        ant_p->NextLocation = ant_p->CurrentLocation;
    }
    else if (nextSquare_r.Ant_p != nullptr) // Manage move toward other ant
    {
        if (nextSquare_r.Ant_p->Team == 0) // If move is toward ally, process possibilities
        {
            // State.Bug << "Ally Blocked Ant" << endl;

            // Check whether ally ant that is blocking current ant is moving toward current ant.
            // If so, a swap between the two ants is possible. 
            auto blockedAntIterator = _antsBlockedByOtherAnts.find(ant_p->Id);
            if ((blockedAntIterator != _antsBlockedByOtherAnts.end()) && (blockedAntIterator->second->Id == nextSquare_r.Ant_p->Id))
            {
                // State.Bug << "Swap Ants " << nLoc.Row << ":" << nLoc.Col << " VS " << ant->CurrentLocation.Row << ":" << ant->CurrentLocation.Col << endl;
                Ant *otherAnt_p = State.Grid[nLoc.Row][nLoc.Col].Ant_p;

                // Send the swap info to the engine
                cout << "o " << ant_p->CurrentLocation.Row << " " << ant_p->CurrentLocation.Col << " " << CDIRECTIONS[ant_p->MoveDirection] << endl;
                cout << "o " << otherAnt_p->CurrentLocation.Row << " " << otherAnt_p->CurrentLocation.Col << " " << CDIRECTIONS[otherAnt_p->MoveDirection] << endl;
                
                // Register the swap in the game state and ant's data
                State.Grid[nLoc.Row][nLoc.Col].Ant_p = ant_p;
                State.Grid[ant_p->CurrentLocation.Row][ant_p->CurrentLocation.Col].Ant_p = otherAnt_p;

                ant_p->NextLocation = blockedAntIterator->second->CurrentLocation;
                blockedAntIterator->second->NextLocation = ant_p->CurrentLocation;
            }
            else
            {
                // If no swap is possible, register the fact that the ant was blocked,
                // in order to unlock the situation if the blocking ally ant is later processed.
                // Only 1 blocked ant is registered at once because in the end, 
                // only one will be able to take the spot.
                _antsBlockedByOtherAnts[nextSquare_r.Ant_p->Id] = ant_p;
                ant_p->NextLocation = ant_p->CurrentLocation;
            }
        }
        else // If move is toward enemy ant, stay in place (both ants should die) 
        {
            ant_p->NextLocation = ant_p->CurrentLocation;
        }
        // else
        // {
        //     State.Bug << "Enemy Blocked Ant" << endl;
        // }
    }
    else
    {
        // Make the ally ant move
        cout << "o " << ant_p->CurrentLocation.Row << " " << ant_p->CurrentLocation.Col << " " << CDIRECTIONS[ant_p->MoveDirection] << endl;
        // State.Bug << "Moving ant of ID " << ant->Id << " to " << nLoc.Row << "/" << nLoc.Col << endl;
        
        // Register move in the game's state and ant's data
        ant_p->NextLocation = nLoc;

        State.Grid[ant_p->CurrentLocation.Row][ant_p->CurrentLocation.Col].Ant_p = nullptr;
        State.Grid[nLoc.Row][nLoc.Col].Ant_p = ant_p;
        
        // Check whether this move unblocked another one
        auto blockedAntIterator = _antsBlockedByOtherAnts.find(ant_p->Id); 
        if (blockedAntIterator != _antsBlockedByOtherAnts.end())
        {
            // State.Bug << "Unlocked Ant";
            MakeMove(blockedAntIterator->second);
        }
    }
}
