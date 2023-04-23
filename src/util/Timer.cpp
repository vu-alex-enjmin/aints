#include "Timer.h"
#include <sys/time.h>

/*
    struct for checking how long it has been since the Start of the Turn.
*/
#ifdef _WIN32 // Windows timer (DON'T USE THIS TIMER UNLESS YOU'RE ON WINDOWS!)

    #include <io.h>
    #include <windows.h>
    Timer::Timer()
    {

    }

    void Timer::Start()
    {
        StartTime = clock();
    }

    double Timer::GetTime()
    {
        CurrentTime = clock();
        return (double)(CurrentTime - StartTime);
    }

#else // Mac/Linux Timer

    Timer::Timer()
    {

    }

    // starts the timer
    void Timer::Start()
    {
        gettimeofday(&TimeVal, NULL);
        StartTime = TimeVal.tv_sec + (TimeVal.tv_usec / 1000000.0);
    }

    // returns how long it has been since the timer was last started in milliseconds
    double Timer::GetTime()
    {
        gettimeofday(&TimeVal, NULL);
        CurrentTime = TimeVal.tv_sec + (TimeVal.tv_usec / 1000000.0);
        return (CurrentTime - StartTime) * 1000.0;
    }

#endif
