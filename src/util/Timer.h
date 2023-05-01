#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>

/*
    struct for checking how long it has been since the Start of the Turn.
*/
#ifdef _WIN32 // Windows timer (DON'T USE THIS TIMER UNLESS YOU'RE ON WINDOWS!)
    #include <io.h>
    #include <windows.h>

    struct Timer
    {
        
        /*
        =========================================
            Attributes
        =========================================
        */
       
        clock_t StartTime, CurrentTime;

        /*
        =========================================
            Methods
        =========================================
        */

        Timer();

        void Start();

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
        double StartTime, CurrentTime;

        Timer();
        
        /*
        =========================================
            Methods
        =========================================
        */

        // starts the timer
        void Start();

        // returns how long it has been since the timer was last started in milliseconds
        double GetTime();
    };
#endif


#endif // TIMER_H_
