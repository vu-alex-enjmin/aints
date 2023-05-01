#include "State.h"

#include <limits>
#include <queue>
#include <cmath>
#include <algorithm>

#include "WrapGridAlgorithm.h"

using namespace std;

// Create state while opening Debug logger if needed 
State::State()
    : GameOver(false)
    , Turn(0)
{
    Bug.Open("./debug.txt");
}

// Destroy state while closing Debug logger if needed 
State::~State()
{
    Bug.Close();
}

// Sets the state up, with game start's information
void State::Setup()
{
    Grid = vector<vector<Square>>(Rows, vector<Square>(Cols, Square()));
}

// Resets all unmanaged containers, all non-water squares,
// and update all managed containers
void State::Reset()
{
    // Reset enemy ants
    for (const Location &enemyLoc_r : EnemyAnts)
    {
        if (Grid[enemyLoc_r.Row][enemyLoc_r.Col].Ant_p != nullptr)
            delete Grid[enemyLoc_r.Row][enemyLoc_r.Col].Ant_p;
    }
    EnemyAnts.clear();

    // Reset food
    Food.clear();

    // Reset grid
    for(int row = 0; row < Rows; row++)
        for(int col = 0; col < Cols; col++)
            if(!Grid[row][col].IsWater)
                Grid[row][col].Reset();
    
    // Update ally ants' location
    Ant *ant_p;
    for (const auto &antPair_r : AllyAnts)
    {
        ant_p = antPair_r.second;
        // Apply move to ant for next turn
        if (ant_p->Decided)
        {
            ant_p->CurrentLocation = ant_p->NextLocation;
            ant_p->NextLocation = Location(-1,-1);
            ant_p->ResetMoveDirection();
        }
        Grid[ant_p->CurrentLocation.Row][ant_p->CurrentLocation.Col].Ant_p = ant_p;
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

// Update Grid Fog of War information
void State::UpdateVisionInformation()
{
    // Create function for resetting visible squares' turns in fog to 0
    auto onVisited = [&](const Location& location, int distance, int direction)
    {
        Grid[location.Row][location.Col].TurnsInFog = 0;
        return false;
    };

    // For each square in view radius of ants, reset its turns in fog value
    for (const auto &antPair_r : AllyAnts)
    {
        Ant *ant_p = antPair_r.second;
        Location antLocation = ant_p->CurrentLocation;

        WrapGridAlgorithm::CircularBreadthFirstSearch(
            antLocation,
            ViewRadius2,
            [&](const Location &loc_r) { return true; },
            onVisited
        );
    }
}

// Checks if pointer points to an ally ant that has not moved yet
bool State::IsAvailableAnt(const Ant *ant_p)
{
    return (ant_p != nullptr) && (ant_p->Team == 0) && (!ant_p->Decided);
}

// Checks if location in the Grid is an ally ant that has not moved yet
bool State::IsAvailableAnt(const Location& location)
{
    Ant *ant_p = Grid[location.Row][location.Col].Ant_p;
    return IsAvailableAnt(ant_p);
}

// Checks if id is a live ally ant that has not moved yet
bool State::IsAvailableAnt(const int id)
{
    // iterate through AllyAnts to find if the ant exists
    auto antIterator = AllyAnts.find(id);
    // when an ant is found, check if it is available
    if(antIterator != AllyAnts.end())
    {
        Ant *ant_p = antIterator->second;
        return IsAvailableAnt(ant_p);
    }
    // no ant found
    return false;
}

// Output function for state, used for whole game state inside output stream
ostream& operator<<(ostream &os_r, const State &state_r)
{
    for (int row = 0; row < state_r.Rows; row++)
    {
        for (int col = 0; col < state_r.Cols; col++)
        {
            if (state_r.Grid[row][col].IsWater)
                os_r << '%';
            else if (state_r.Grid[row][col].IsFood)
                os_r << '*';
            else if (state_r.Grid[row][col].IsHill)
                os_r << (char)('A' + state_r.Grid[row][col].HillPlayer);
            else if (state_r.Grid[row][col].Ant_p != nullptr)
                os_r << (char)('a' + state_r.Grid[row][col].Ant_p->Team);
            else if (state_r.Grid[row][col].TurnsInFog == 0)
                os_r << '.';
            else
                os_r << '?';
        }
        os_r << endl;
    }

    return os_r;
}

// Input function, used to initialize/update state
// after receiving game information
istream& operator>>(istream &is_r, State &state_r)
{
    int row, col, player;
    string inputType, junk;
    Ant *ant_p;

    // Finds out which turn it is
    while (is_r >> inputType)
    {
        if (inputType == "end")
        {
            state_r.GameOver = 1;
            break;
        }
        else if (inputType == "turn")
        {
            is_r >> state_r.Turn;
            break;
        }
        else // Unknown line
            getline(is_r, junk);
    }

    if (state_r.Turn == 0)
    {
        // Reads game parameters
        while (is_r >> inputType)
        {
            if (inputType == "loadtime")
                is_r >> state_r.LoadTime;
            else if (inputType == "turntime")
                is_r >> state_r.TurnTime;
            else if (inputType == "rows")
                is_r >> state_r.Rows;
            else if (inputType == "cols")
                is_r >> state_r.Cols;
            else if (inputType == "turns")
                is_r >> state_r.MaxTurns;
            else if (inputType == "player_seed")
                is_r >> state_r.Seed;
            else if (inputType == "viewradius2")
            {
                is_r >> state_r.ViewRadius2;
                state_r.ViewRadius = sqrt(state_r.ViewRadius2);
            }
            else if (inputType == "attackradius2")
            {
                is_r >> state_r.AttackRadius2;
                state_r.AttackRadius = sqrt(state_r.AttackRadius2);
            }
            else if (inputType == "spawnradius2")
            {
                is_r >> state_r.SpawnRadius2;
                state_r.SpawnRadius = sqrt(state_r.SpawnRadius2);
            }
            else if (inputType == "ready") // End of parameter input
            {
                state_r.Timer.Start();
                break;
            }
            else    // Unknown line
                getline(is_r, junk);
        }
    }
    else
    {
        // Reads information about the current Turn
        while (is_r >> inputType)
        {
            // state.Bug << inputType << endl;
            if (inputType == "w") // Water square
            {
                is_r >> row >> col;
                state_r.Grid[row][col].IsWater = 1;
            }
            else if (inputType == "f") // Food square
            {
                is_r >> row >> col;
                state_r.Grid[row][col].IsFood = 1;
                state_r.Food.push_back(Location(row, col));
            }
            else if (inputType == "a") // Live ant square
            {
                is_r >> row >> col >> player;
                if (state_r.Grid[row][col].Ant_p == nullptr) // new ant
                {
                    ant_p = new Ant(player, Location(row, col));
                    state_r.Bug << "Ant added" << ant_p->Id << " - (" << ant_p->Team << ") "<< row << "/"<< col << endl;
                    state_r.Grid[row][col].Ant_p = ant_p;
                    
                    if (player == 0)
                    {
                        
                        state_r.AllyAnts[ant_p->Id] = ant_p;
                    }
                    else
                    {
                        state_r.EnemyAnts.push_back(Location(row, col));
                    }
                }
            }
            else if (inputType == "d") // Dead ant square
            {
                is_r >> row >> col >> player;
                if ((player == 0) &&
                    (state_r.Grid[row][col].Ant_p != nullptr) &&
                    (state_r.Grid[row][col].Ant_p->Team == 0))
                {
                    try
                    {
                        // Attempt to erased ally ant from tracked allies if one died
                        ant_p = state_r.AllyAnts.at( state_r.Grid[row][col].Ant_p->Id );
                        state_r.AllyAnts.erase(ant_p->Id);
                        delete ant_p;
                    }
                    catch (const exception & e) 
                    {
                        // Log error if there is a problem when deleting an ally ant
                        state_r.Bug << "Dead "<< 
                            e.what() << ": ID - " << 
                            state_r.Grid[row][col].Ant_p->Id << " " << 
                            row << "/" << col << endl;
                    }

                    state_r.Grid[row][col].Ant_p = nullptr;
                }
            }
            else if (inputType == "h")
            {
                is_r >> row >> col >> player;
                state_r.Grid[row][col].IsHill = 1;
                state_r.Grid[row][col].HillPlayer = player;
                // Update state's hills info
                if (player == 0)
                    state_r.MyHills.insert(Location(row, col));
                else
                    state_r.EnemyHills.insert(Location(row, col));

            }
            else if (inputType == "players") // Player information
                is_r >> state_r.NoPlayers;
            else if (inputType == "scores") // Score information
            {
                state_r.Scores = vector<double>(state_r.NoPlayers, 0.0);
                for(int p = 0; p < state_r.NoPlayers; p++)
                    is_r >> state_r.Scores[p];
            }
            else if (inputType == "go") // End of Turn input
            {
                if (state_r.GameOver)
                    is_r.setstate(std::ios::failbit);
                else
                    state_r.Timer.Start();
                break;
            }
            else // Unknown line
                getline(is_r, junk);
        }
    }

    return is_r;
}
