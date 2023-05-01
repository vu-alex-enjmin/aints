#include "Bug.h"

// Create the object used for debug logging
Bug::Bug()
{
    
}

// Opens the specified file for debug logs
void Bug::Open(const std::string &filename_r)
{
    // Open the file only if DEBUG keyword is defined
    #ifdef DEBUG
        File.open(filename_r.c_str());
    #endif
}

// Closes the ofstream
void Bug::Close()
{
    // Close the file only if DEBUG keyword is defined
    #ifdef DEBUG
        File.close();
    #endif
}

// Output function
Bug& operator<<(Bug &bug_r, std::ostream& (*manipulator)(std::ostream&))
{
    // Write into file if DEBUG keyword is defined
    // else, don't do anything and return the Bug instance
    #ifdef DEBUG
        bug_r.File << manipulator;
    #endif

    return bug_r;
}
