#include "Timer.h"
#include <sys/time.h>

#ifdef _WIN32 // Windows timer

    #include <io.h>
    #include <windows.h>

    // Creates the timer
    Timer::Timer()
    {

    }

    // Starts the timer
    void Timer::Start()
    {
        StartTime = clock();
    }

    // Returns how long it has been since the timer was last started in milliseconds
    double Timer::GetTime()
    {
        CurrentTime = clock();
        return (double)(CurrentTime - StartTime);
    }

#else // Mac/Linux Timer

    // Creates the timer
    Timer::Timer()
    {

    }

    // Starts the timer
    void Timer::Start()
    {
        gettimeofday(&TimeVal, NULL);
        StartTime = TimeVal.tv_sec + (TimeVal.tv_usec / 1000000.0);
    }

    // Returns how long it has been since the timer was last started in milliseconds
    double Timer::GetTime()
    {
        gettimeofday(&TimeVal, NULL);
        CurrentTime = TimeVal.tv_sec + (TimeVal.tv_usec / 1000000.0);
        return (CurrentTime - StartTime) * 1000.0;
    }

#endif
