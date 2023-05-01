#include "State.h"

#include <limits>
#include <queue>
#include <algorithm>

#include "WrapGridAlgorithm.h"

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

/*
    // TODO : update this function header comment

    This function will update update the lastSeen value for any squares currently
    visible by one of your live ants.

    BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
    THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
    A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
    IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::UpdateVisionInformation()
{
    auto onVisited = [&](const Location& location, int distance, int direction)
    {
        Grid[location.Row][location.Col].TurnsInFog = 0;
        return false;
    };

    for (const auto &antPair : AllyAnts)
    {
        Ant* ant = antPair.second;
        Location antLocation = ant->CurrentLocation;

        WrapGridAlgorithm::CircularBreadthFirstSearch(
            antLocation,
            ViewRadius2,
            [&](const Location &loc) { return true; },
            onVisited
        );
    }
}

// TODO : move to Bot class and use proper functions instead
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
        explLoc = WrapGridAlgorithm::GetLocation(startLoc, exploreDir);
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
                        nextLoc = WrapGridAlgorithm::GetLocation(currLoc, d);

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
        return WrapGridAlgorithm::GetLocation(startLoc, bestDirection);
    }
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

// Checks if pointer points to an ally ant that has not moved yet
bool State::IsAvailableAnt(const Ant* ant)
{
    return (ant != nullptr) && (ant->Team == 0) && (!ant->Decided);
}

// Checks if location in the Grid is an ally ant that has not moved yet
bool State::IsAvailableAnt(const Location& location)
{
    Ant* ant = Grid[location.Row][location.Col].Ant;
    return IsAvailableAnt(ant);
}

// Checks if id is a live ally ant that has not moved yet
bool State::IsAvailableAnt(const int id)
{
    // iterate through AllyAnts to find if the ant exists
    auto antIterator = AllyAnts.find(id);
    // when an ant is found, check if it is available
    if(antIterator != AllyAnts.end())
    {
        Ant* ant = antIterator->second;
        return IsAvailableAnt(ant);
    }
    // no ant found
    return false;
}
