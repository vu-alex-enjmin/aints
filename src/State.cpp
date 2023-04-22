#include "State.h"
#include <limits>
#include <queue>
#include <algorithm>

using namespace std;

//constructor
State::State()
    : GameOver(false)
    , Turn(0)
{
    Bug.Open("./debug.txt");
}

//deconstructor
State::~State()
{
    Bug.Close();
}

//sets the state up
void State::Setup()
{
    Grid = vector<vector<Square>>(Rows, vector<Square>(Cols, Square()));
}

//resets all non-water squares to land and clears the bots ant vector
void State::Reset()
{
    MyAnts.clear();
    EnemyAnts.clear();
    MyHills.clear();
    EnemyHills.clear();
    Food.clear();
    for(int row=0; row<Rows; row++)
        for(int col=0; col<Cols; col++)
            if(!Grid[row][col].IsWater)
                Grid[row][col].Reset();
}

//returns the euclidean distance between two locations with the edges wrapped
double State::Distance(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.Row-loc2.Row),
        d2 = abs(loc1.Col-loc2.Col),
        dr = min(d1, Rows-d1),
        dc = min(d2, Cols-d2);
    return sqrt(dr*dr + dc*dc);
}

//returns the new location from moving in a given direction with the edges wrapped
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

    // Prime algorithm
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
    Location sLoc, cLoc, nLoc;

    for(int a=0; a<(int) MyAnts.size(); a++)
    {
        sLoc = MyAnts[a];
        locQueue.push(sLoc);

        std::vector<std::vector<bool> > visited(Rows, std::vector<bool>(Cols, 0));
        Grid[sLoc.Row][sLoc.Col].TurnsInFog = 0;
        visited[sLoc.Row][sLoc.Col] = 1;

        while(!locQueue.empty())
        {
            cLoc = locQueue.front();
            locQueue.pop();

            for(int d=0; d<TDIRECTIONS; d++)
            {
                nLoc = GetLocation(cLoc, d);

                if(!visited[nLoc.Row][nLoc.Col] && Distance(sLoc, nLoc) <= ViewRadius)
                {
                    Grid[nLoc.Row][nLoc.Col].TurnsInFog = 0;
                    locQueue.push(nLoc);
                }
                visited[nLoc.Row][nLoc.Col] = 1;
            }
        }
    }
}

Location State::BreadthFirstSearch(const Location &startLoc, int* outDirection, int range, function<bool(const Location&)> const &stopPredicate, bool ignoreWater)
{
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
        if ((Grid[explLoc.Row][explLoc.Col].Ant.Team == -1) && 
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
                if(distances[currLoc.Row][currLoc.Col] < stopRange)
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
    if(bestScore == 0)
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
    for(int row=0; row<state.Rows; row++)
    {
        for(int col=0; col<state.Cols; col++)
        {
            if(state.Grid[row][col].IsWater)
                os << '%';
            else if(state.Grid[row][col].IsFood)
                os << '*';
            else if(state.Grid[row][col].IsHill)
                os << (char)('A' + state.Grid[row][col].HillPlayer);
            else if(state.Grid[row][col].Ant.Team >= 0)
                os << (char)('a' + state.Grid[row][col].Ant.Team);
            else if(state.Grid[row][col].TurnsInFog == 0)
                os << '.';
            else
                os << '?';
        }
        os << endl;
    }

    return os;
}

//input function
istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            state.GameOver = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.Turn;
            break;
        }
        else //unknown line
            getline(is, junk);
    }

    if(state.Turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.LoadTime;
            else if(inputType == "turntime")
                is >> state.TurnTime;
            else if(inputType == "rows")
                is >> state.Rows;
            else if(inputType == "cols")
                is >> state.Cols;
            else if(inputType == "turns")
                is >> state.MaxTurns;
            else if(inputType == "player_seed")
                is >> state.Seed;
            else if(inputType == "viewradius2")
            {
                is >> state.ViewRadius2;
                state.ViewRadius = sqrt(state.ViewRadius2);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.AttackRadius2;
                state.AttackRadius = sqrt(state.AttackRadius2);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.SpawnRadius2;
                state.SpawnRadius = sqrt(state.SpawnRadius2);
            }
            else if(inputType == "ready") //end of parameter input
            {
                state.Timer.Start();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        //reads information about the current Turn
        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.Grid[row][col].IsWater = 1;
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                state.Grid[row][col].IsFood = 1;
                state.Food.push_back(Location(row, col));
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                state.Grid[row][col].Ant = Ant(player, Location(row, col));
                if(player == 0)
                    state.MyAnts.push_back(Location(row, col));
                else
                    state.EnemyAnts.push_back(Location(row, col));
            }
            else if(inputType == "d") //dead ant square
            {
                is >> row >> col >> player;
            }
            else if(inputType == "h")
            {
                is >> row >> col >> player;
                state.Grid[row][col].IsHill = 1;
                state.Grid[row][col].HillPlayer = player;
                if(player == 0)
                    state.MyHills.push_back(Location(row, col));
                else
                    state.EnemyHills.push_back(Location(row, col));

            }
            else if(inputType == "players") //player information
                is >> state.NoPlayers;
            else if(inputType == "scores") //score information
            {
                state.Scores = vector<double>(state.NoPlayers, 0.0);
                for(int p=0; p<state.NoPlayers; p++)
                    is >> state.Scores[p];
            }
            else if(inputType == "go") //end of Turn input
            {
                if(state.GameOver)
                    is.setstate(std::ios::failbit);
                else
                    state.Timer.Start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
    }

    return is;
}
