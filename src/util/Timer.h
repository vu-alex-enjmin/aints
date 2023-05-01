#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>

// Struct for checking how long it has been since the Start of the Turn.
#ifdef _WIN32 // Windows timer
    #include <io.h>
    #include <windows.h>

    struct Timer
    {
        /*
        =========================================
            Attributes
        =========================================
        */
        // Time at which the timer was started
        clock_t StartTime;
        // Current timer time
        clock_t CurrentTime;

        /*
        =========================================
            Methods
        =========================================
        */

        // Creates the timer
        Timer();
        // Starts the timer
        void Start();
        // Returns how long it has been since the timer was last started in milliseconds
        double GetTime();
    };

#else // Mac/Linux Timer
    struct Timer
    {
        
        /*
        =========================================
            Attributes
        =========================================
        */

        timeval TimeVal;
        // Time at which the timer was started
        double StartTime;
        // Current timer time
        double CurrentTime;
        
        /*
        =========================================
            Methods
        =========================================
        */

        // Creates the timer
        Timer();
        // Starts the timer
        void Start();
        // Returns how long it has been since the timer was last started in milliseconds
        double GetTime();
    };
#endif


#endif // TIMER_H_
