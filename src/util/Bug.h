#ifndef BUG_H_
#define BUG_H_

#include <fstream>

// Struct used for debug logs.
// To be used like an output stream. 
struct Bug
{
    public:
        /*
        =========================================
            Attributes
        =========================================
        */
        // File in which to write debug logs
        // (cout does not work because it is used for sending moves)
        std::ofstream File;

        
        /*
        =========================================
            Methods
        =========================================
        */

        // Create the object used for debug logging
        Bug();

        // Opens the specified file for debug logs
        void Open(const std::string &filename_r);

        // Closes the ofstream
        void Close();
};

// Output function for endl
Bug& operator<<(Bug &bug_r, std::ostream& (*manipulator)(std::ostream&));

// Output function
template <class T>
Bug& operator<<(Bug &bug_r, const T &t_r)
{
    // Write into file if DEBUG keyword is defined
    // else, don't do anything and return the Bug instance
    #ifdef DEBUG
        bug_r.File << t_r;
    #endif
    
    return bug_r;
};

#endif // BUG_H_
