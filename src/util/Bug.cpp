#include "Bug.h"

Bug::Bug()
{
    
}

void Bug::Open(const std::string &filename)
{
    #ifdef DEBUG
        File.open(filename.c_str());
    #endif
}

void Bug::Close()
{
    #ifdef DEBUG
        File.close();
    #endif
}

Bug& operator<<(Bug &bug, std::ostream& (*manipulator)(std::ostream&))
{
    #ifdef DEBUG
        bug.File << manipulator;
    #endif

    return bug;
}
