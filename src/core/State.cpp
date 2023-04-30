#include "State.h"

#include <limits>
#include <queue>
#include <algorithm>

using namespace std;

// Constructor
State::State()
    : GameOver(false)
    , Turn(0)
{
    Bug.Open("./debug.txt");
}

// Deconstructor
State::~State()
{
    Bug.Close();
}

// Sets the state up
void State::Setup()
{
    Grid = vector<vector<Square>>(Rows, vector<Square>(Cols, Square()));
}

// Resets all non-water squares to land and clears the bots ant vector
void State::Reset()
{
    Bug << "Reset 1" << endl;
    for (const Location &enemyLoc : EnemyAnts)
    {
        if (Grid[enemyLoc.Row][enemyLoc.Col].Ant != nullptr)
            delete Grid[enemyLoc.Row][enemyLoc.Col].Ant;
    }

    Bug << "Reset 2" << endl;
    EnemyAnts.clear();
    Food.clear();
    NewlyDeadAllyAnts.clear();

    Bug << "Reset 3" << endl;
    for(int row = 0; row < Rows; row++)
        for(int col = 0; col < Cols; col++)
            if(!Grid[row][col].IsWater)
                Grid[row][col].Reset();
    
    Bug << "Reset 4" << endl;
    Ant* ant;
    for (const auto &antPair : AllyAnts)
    {
        ant = antPair.second;
        // Bug << "Reset Id " << ant->Id << " " << antPair.first << endl;
        // Apply move to ant for next turn
        if (ant->Decided)
        {
            ant->CurrentLocation = ant->NextLocation;
            ant->NextLocation = Location(-1,-1);
            ant->ResetMoveDirection();
            // Bug << "   Move ant " << ant->Id << " to " << ant->CurrentLocation.Row << "/" << ant->CurrentLocation.Col << endl;
        }
        Grid[ant->CurrentLocation.Row][ant->CurrentLocation.Col].Ant = ant;
        // Bug << "Reset Done " << ant->Id << endl; 
    }
}

void State::UpdateHillInformation()
{
    // Check if ally hills got deleted
    for (auto it = MyHills.begin(); it != MyHills.end(); ) 
    {
        if ((Grid[it->Row][it->Col].TurnsInFog == 0) &&
            (Grid[it->Row][it->Col].HillPlayer == -1)) 
        {
            Bug << "Hill Razed at "<<it->Row << "/" << it->Col << endl;
            MyHills.erase(it++);
        }
        else 
        {
            ++it;
        }
    }

    // Check if enemy hills got deleted
    for (auto it = EnemyHills.begin(); it != EnemyHills.end(); ) 
    {
        if ((Grid[it->Row][it->Col].TurnsInFog == 0) &&
            (Grid[it->Row][it->Col].HillPlayer == -1)) 
        {
            Bug << "Hill Razed at "<<it->Row << "/" << it->Col << endl;
            EnemyHills.erase(it++);
        }
        else 
        {
            ++it;
        }
    }
}

// Returns the euclidean distance between two locations with the edges wrapped
double State::Distance(const Location &loc1, const Location &loc2)
{
    return sqrt(Distance2(loc1, loc2));
}

// Returns the euclidean square distance between two locations with the edges wrapped
int State::Distance2(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.Row-loc2.Row),
        d2 = abs(loc1.Col-loc2.Col),
        dr = min(d1, Rows-d1),
        dc = min(d2, Cols-d2);
    return dr*dr + dc*dc;
}

// Returns the new location from moving in a given direction with the edges wrapped
Location State::GetLocation(const Location &loc, int direction)
{
    return Location( (loc.Row + DIRECTIONS[direction][0] + Rows) % Rows,
                     (loc.Col + DIRECTIONS[direction][1] + Cols) % Cols );
}

vector<int> State::AStar(const Location &startLoc, const Location &targetLoc)
{
    // Initialization of collections
    //  actual distance to target
    vector<vector<int>> distances(Rows, vector<int>(Cols, numeric_limits<int>::max()));
    //  score (distance to target + heuristic)
    vector<vector<int>> scores(Rows, vector<int>(Cols, numeric_limits<int>::max()));
    //  directions taken to reach a given location (used to reconstruct the path)
    vector<vector<int>> directions(Rows, vector<int>(Cols, -1));
    auto comparator = [&](const Location a, const Location b)
    {
        return scores[a.Row][a.Col] > scores[b.Row][b.Col];
    };
    priority_queue<Location, vector<Location>, decltype(comparator)> locQueue(comparator);

    // Prime the algorithm
    locQueue.push(startLoc);
    scores[startLoc.Row][startLoc.Col] = 0;
    distances[startLoc.Row][startLoc.Col] = 0;

    // Search for path
    Location currLoc, nextLoc;
    int currLocDist, nextLocDist;
    while (!locQueue.empty())
    {
        currLoc = locQueue.top();
        // Compute path that was taken to reach target and return it
        if (currLoc == targetLoc)
        {
            vector<int> pathDirections(distances[currLoc.Row][currLoc.Col]);

            while (currLoc != startLoc)
            {
                pathDirections.push_back(directions[currLoc.Row][currLoc.Col]);
                currLoc = GetLocation(currLoc, (directions[currLoc.Row][currLoc.Col] + TDIRECTIONS / 2) % TDIRECTIONS);
            }
            reverse(pathDirections.begin(), pathDirections.end());

            return pathDirections;
        }
        
        // Add/Update unvisited neighbours
        locQueue.pop();
        currLocDist = distances[currLoc.Row][currLoc.Col];
        for (int d = 0; d < TDIRECTIONS; d++)
        {
            nextLoc = GetLocation(currLoc, d);
            // If next location is water, skip it
            if (Grid[nextLoc.Row][nextLoc.Col].IsWater)
                continue;

            nextLocDist = currLocDist + 1;
            if (nextLocDist < distances[nextLoc.Row][nextLoc.Col])
            {
                directions[nextLoc.Row][nextLoc.Col] = d;
                distances[nextLoc.Row][nextLoc.Col] = nextLocDist;
                scores[nextLoc.Row][nextLoc.Col] = nextLocDist + ManhattanDistance(nextLoc, targetLoc);
                locQueue.push(nextLoc);
            }
        }
    }

    // If no path is found, a path containing (-1) is returned
    return vector<int>(1, -1);
}

/*
    This function will update update the lastSeen value for any squares currently
    visible by one of your live ants.

    BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
    THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
    A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
    IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::UpdateVisionInformation()
{
    std::queue<Location> locQueue;
    Location startLoc, currentLoc, nextLoc;

    Ant* ant;
    for (const auto &antPair : AllyAnts)
    {
        ant = antPair.second;
        startLoc = ant->CurrentLocation;

        locQueue.push(startLoc);

        std::vector<std::vector<bool> > visited(Rows, std::vector<bool>(Cols, 0));
        Grid[startLoc.Row][startLoc.Col].TurnsInFog = 0;
        visited[startLoc.Row][startLoc.Col] = 1;

        while (!locQueue.empty())
        {
            currentLoc = locQueue.front();
            locQueue.pop();

            for (int d = 0; d < TDIRECTIONS; d++)
            {
                nextLoc = GetLocation(currentLoc, d);

                if(!visited[nextLoc.Row][nextLoc.Col] && Distance(startLoc, nextLoc) <= ViewRadius)
                {
                    Grid[nextLoc.Row][nextLoc.Col].TurnsInFog = 0;
                    locQueue.push(nextLoc);
                }
                visited[nextLoc.Row][nextLoc.Col] = 1;
            }
        }
    }
}

Location State::BreadthFirstSearch(const Location &startLoc, int* outDirection, int range, function<bool(const Location&)> const &stopPredicate, bool ignoreWater)
{
    if (stopPredicate(startLoc))
    {
        return startLoc;
    }

    std::queue<Location> locQueue;
    Location currLoc, nextLoc;
    int nextDist;

    locQueue.push(startLoc);

    std::vector<std::vector<int>> distances(Rows, std::vector<int>(Cols, -1));
    distances[startLoc.Row][startLoc.Col] = 0;

    while (!locQueue.empty())
    {
        currLoc = locQueue.front();
        locQueue.pop();
        nextDist = distances[currLoc.Row][currLoc.Col] + 1;

        for (int d = 0; d < TDIRECTIONS; d++)
        {
            nextLoc = GetLocation(currLoc, d);

            if ((distances[nextLoc.Row][nextLoc.Col] == -1) &&
                (ignoreWater || !Grid[nextLoc.Row][nextLoc.Col].IsWater) &&
                (nextDist <= range))
            {
                if (stopPredicate(nextLoc))
                {
                    if (outDirection != nullptr)
                    {
                        *outDirection = (d + TDIRECTIONS / 2) % TDIRECTIONS;
                    }
                    return nextLoc;
                }
                locQueue.push(nextLoc);
            }
            distances[nextLoc.Row][nextLoc.Col] = nextDist;
        }
    }

    return Location(-1, -1);
}

void State::BreadthFirstSearchAll(const Location &startLoc, int range, function<void(const Location&)> const &onVisited, bool ignoreWater)
{
    std::queue<Location> locQueue;
    locQueue.push(startLoc);

    std::vector<std::vector<int>> distances(Rows, std::vector<int>(Cols, -1));
    distances[startLoc.Row][startLoc.Col] = 0;

    Location currLoc, nextLoc;
    int nextDist;
    while (!locQueue.empty())
    {
        currLoc = locQueue.front();
        locQueue.pop();
        nextDist = distances[currLoc.Row][currLoc.Col] + 1;

        for (int d = 0; d < TDIRECTIONS; d++)
        {
            nextLoc = GetLocation(currLoc, d);

            if ((distances[nextLoc.Row][nextLoc.Col] == -1) &&
                (ignoreWater || !Grid[nextLoc.Row][nextLoc.Col].IsWater) &&
                (nextDist <= range))
            {
                onVisited(nextLoc);
                locQueue.push(nextLoc);
            }
            distances[nextLoc.Row][nextLoc.Col] = nextDist;
        }
    }
}

void State::MultiBreadthFirstSearchAll(const std::vector<Location> &startLocs, int range, function<bool(const Location&, const int)> const &onVisited, bool ignoreWater)
{
    std::queue<Location> locQueue;

    std::vector<std::vector<int>> distances(Rows, std::vector<int>(Cols, -1));
     for(auto startLoc : startLocs)
     {
        if (onVisited(startLoc, 0))
        {
            return;
        }
        locQueue.push(startLoc);
        distances[startLoc.Row][startLoc.Col] = 0;
     }

    Location currLoc, nextLoc;
    int nextDist;
    while (!locQueue.empty())
    {
        currLoc = locQueue.front();
        locQueue.pop();
        nextDist = distances[currLoc.Row][currLoc.Col] + 1;

        for (int d = 0; d < TDIRECTIONS; d++)
        {
            nextLoc = GetLocation(currLoc, d);

            if ((distances[nextLoc.Row][nextLoc.Col] == -1) &&
                (ignoreWater || !Grid[nextLoc.Row][nextLoc.Col].IsWater) &&
                (nextDist <= range))
            {
                if (onVisited(nextLoc, nextDist))
                {
                    return;
                }
                locQueue.push(nextLoc);
            }
            distances[nextLoc.Row][nextLoc.Col] = nextDist;
        }
    }
}

Location State::SearchMostFogged(const Location &startLoc, int* outDirection, int stopRange)
{
    Location currLoc, nextLoc, explLoc;
    int currentScore;

    int bestDirection;
    int bestScore = 0;

    int nextDist;
    for (int exploreDir = 0; exploreDir < TDIRECTIONS; exploreDir++)
    {
        currentScore = 0;
        explLoc = GetLocation(startLoc, exploreDir);
        if ((Grid[explLoc.Row][explLoc.Col].Ant == nullptr) && 
            (!Grid[explLoc.Row][explLoc.Col].IsWater))
        {
            std::queue<Location> locQueue;
            locQueue.push(explLoc);
            
            std::vector<std::vector<int>> distances(Rows, std::vector<int>(Cols, -1));
            distances[explLoc.Row][explLoc.Col] = 0;

            while (!locQueue.empty())
            {
                currLoc = locQueue.front();
                locQueue.pop();
                currentScore += Grid[currLoc.Row][currLoc.Col].TurnsInFog;
                if (distances[currLoc.Row][currLoc.Col] < stopRange)
                {
                    nextDist = distances[currLoc.Row][currLoc.Col] + 1;
                    for (int d = 0; d < TDIRECTIONS; d++)
                    {
                        nextLoc = GetLocation(currLoc, d);

                        if ((distances[nextLoc.Row][nextLoc.Col] == -1) &&
                            (!Grid[nextLoc.Row][nextLoc.Col].IsWater))
                        {
                            locQueue.push(nextLoc);
                            distances[nextLoc.Row][nextLoc.Col] = nextDist;
                        }
                    }
                }
            }
            if (currentScore >= bestScore)
            {
                bestDirection = exploreDir;
                bestScore = currentScore;
            }
        }
    }
    
    if (bestScore == 0)
    {
        return Location(-1, -1);
    }
    else
    {
        *outDirection = bestDirection;
        return GetLocation(startLoc, bestDirection);
    }
}

double State::ManhattanDistance(const Location &loc1, const Location &loc2)
{
    int rowDist = abs(loc1.Row-loc2.Row);
    int colDist = abs(loc1.Col-loc2.Col);
    // Evaluating distance both ways (since the map loops)
    int bestRowDist = min(rowDist, Rows-rowDist);
    int bestColDist = min(colDist, Cols-colDist);
    
    return bestRowDist + bestColDist;
}

/*
    This is the output function for a state. It will add a char map
    representation of the state to the output stream passed to it.

    For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const State &state)
{
    for (int row = 0; row < state.Rows; row++)
    {
        for (int col = 0; col < state.Cols; col++)
        {
            if (state.Grid[row][col].IsWater)
                os << '%';
            else if (state.Grid[row][col].IsFood)
                os << '*';
            else if (state.Grid[row][col].IsHill)
                os << (char)('A' + state.Grid[row][col].HillPlayer);
            else if (state.Grid[row][col].Ant != nullptr)
                os << (char)('a' + state.Grid[row][col].Ant->Team);
            else if (state.Grid[row][col].TurnsInFog == 0)
                os << '.';
            else
                os << '?';
        }
        os << endl;
    }

    return os;
}

// Input function
istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;
    Ant* ant;

    // Finds out which turn it is
    while (is >> inputType)
    {
        if (inputType == "end")
        {
            state.GameOver = 1;
            break;
        }
        else if (inputType == "turn")
        {
            is >> state.Turn;
            break;
        }
        else // Unknown line
            getline(is, junk);
    }

    if (state.Turn == 0)
    {
        // Reads game parameters
        while (is >> inputType)
        {
            if (inputType == "loadtime")
                is >> state.LoadTime;
            else if (inputType == "turntime")
                is >> state.TurnTime;
            else if (inputType == "rows")
                is >> state.Rows;
            else if (inputType == "cols")
                is >> state.Cols;
            else if (inputType == "turns")
                is >> state.MaxTurns;
            else if (inputType == "player_seed")
                is >> state.Seed;
            else if (inputType == "viewradius2")
            {
                is >> state.ViewRadius2;
                state.ViewRadius = sqrt(state.ViewRadius2);
            }
            else if (inputType == "attackradius2")
            {
                is >> state.AttackRadius2;
                state.AttackRadius = sqrt(state.AttackRadius2);
            }
            else if (inputType == "spawnradius2")
            {
                is >> state.SpawnRadius2;
                state.SpawnRadius = sqrt(state.SpawnRadius2);
            }
            else if (inputType == "ready") // End of parameter input
            {
                state.Timer.Start();
                break;
            }
            else    // Unknown line
                getline(is, junk);
        }
    }
    else
    {
        // Reads information about the current Turn
        while (is >> inputType)
        {
            // state.Bug << inputType << endl;
            if (inputType == "w") // Water square
            {
                is >> row >> col;
                state.Grid[row][col].IsWater = 1;
            }
            else if (inputType == "f") // Food square
            {
                is >> row >> col;
                state.Grid[row][col].IsFood = 1;
                state.Food.push_back(Location(row, col));
            }
            else if (inputType == "a") // Live ant square
            {
                is >> row >> col >> player;
                if (state.Grid[row][col].Ant == nullptr) // new ant
                {
                    ant = new Ant(player, Location(row, col));
                    state.Bug << "Ant added" << ant->Id << " - (" << ant->Team << ") "<< row << "/"<< col << endl;
                    state.Grid[row][col].Ant = ant;
                    
                    if (player == 0)
                    {
                        
                        state.AllyAnts[ant->Id] = ant;
                    }
                    else
                    {
                        state.EnemyAnts.push_back(Location(row, col));
                    }
                }
            }
            else if (inputType == "d") // Dead ant square
            {
                is >> row >> col >> player;
                if ((player == 0) &&
                    (state.Grid[row][col].Ant != nullptr) &&
                    (state.Grid[row][col].Ant->Team == 0))
                {
                    try
                    {
                        state.Bug << "Dead ID - " << 
                            state.Grid[row][col].Ant->Id << " " << 
                            row << "/" << col << endl;

                        ant = state.AllyAnts.at( state.Grid[row][col].Ant->Id );
                        state.AllyAnts.erase(ant->Id);

                        delete ant;
                    }
                    catch (const exception & e) 
                    {
                        state.Bug << "Dead "<< 
                            e.what() << ": ID - " << 
                            state.Grid[row][col].Ant->Id << " " << 
                            row << "/" << col << endl;
                    }

                    state.Grid[row][col].Ant = nullptr;
                }
            }
            else if (inputType == "h")
            {
                is >> row >> col >> player;
                state.Grid[row][col].IsHill = 1;
                state.Grid[row][col].HillPlayer = player;
                if (player == 0)
                    state.MyHills.insert(Location(row, col));
                else
                    state.EnemyHills.insert(Location(row, col));

            }
            else if (inputType == "players") // Player information
                is >> state.NoPlayers;
            else if (inputType == "scores") // Score information
            {
                state.Scores = vector<double>(state.NoPlayers, 0.0);
                for(int p = 0; p < state.NoPlayers; p++)
                    is >> state.Scores[p];
            }
            else if (inputType == "go") // End of Turn input
            {
                if (state.GameOver)
                    is.setstate(std::ios::failbit);
                else
                    state.Timer.Start();
                break;
            }
            else // Unknown line
                getline(is, junk);
        }
    }

    return is;
}
