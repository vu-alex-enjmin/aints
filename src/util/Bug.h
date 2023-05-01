#ifndef BUG_H_
#define BUG_H_

#include <fstream>

#ifndef DEBUG
    // #define DEBUG
#endif

/*
    struct for debugging - this is gross but can be used pretty much like an ofstream,
                           except the debug messages are stripped while compiling if
                           DEBUG is not defined.
    example:
        Bug bug;
        bug.open("./debug.txt");
        bug << state << endl;
        bug << "testing" << 2.0 << '%' << endl;
        bug.close();
*/
struct Bug
{
    
    /*
    =========================================
        Attributes
    =========================================
    */

    std::ofstream File;

    
    /*
    =========================================
        Methods
    =========================================
    */

    Bug();

    // opens the specified file
    void Open(const std::string &filename);

    // closes the ofstream
    void Close();
};

// output function for endl
Bug& operator<<(Bug &bug, std::ostream& (*manipulator)(std::ostream&));

// output function
template <class T>
Bug& operator<<(Bug &bug, const T &t)
{
    #ifdef DEBUG
        bug.File << t;
    #endif
    
    return bug;
};

#endif // BUG_H_
