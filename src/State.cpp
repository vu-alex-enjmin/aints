#include "State.h"

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

//outputs move information to the engine
void State::MakeMove(const Location &loc, int direction)
{
    cout << "o " << loc.Row << " " << loc.Col << " " << CDIRECTIONS[direction] << endl;

    Location nLoc = GetLocation(loc, direction);
    Grid[nLoc.Row][nLoc.Col].Ant = Grid[loc.Row][loc.Col].Ant;
    Grid[loc.Row][loc.Col].Ant = -1;
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
        Grid[sLoc.Row][sLoc.Col].IsVisible = 1;
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
                    Grid[nLoc.Row][nLoc.Col].IsVisible = 1;
                    locQueue.push(nLoc);
                }
                visited[nLoc.Row][nLoc.Col] = 1;
            }
        }
    }
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
            else if(state.Grid[row][col].Ant >= 0)
                os << (char)('a' + state.Grid[row][col].Ant);
            else if(state.Grid[row][col].IsVisible)
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
                is >> state.ViewRadius;
                state.ViewRadius = sqrt(state.ViewRadius);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.AttackRadius;
                state.AttackRadius = sqrt(state.AttackRadius);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.SpawnRadius;
                state.SpawnRadius = sqrt(state.SpawnRadius);
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
                state.Grid[row][col].Ant = player;
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
