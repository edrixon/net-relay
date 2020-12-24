#include "types.h"
#include "globals.h"

// Decrement all the system timers
void decrementTimers()
{
    int c;

    for(c = 0; c < MAXTIMERS; c++)
    {
        if(timers[c].count)
        {
            timers[c].count--;
        }
    }
}

// Reset a system timer
void resetTimer(int timer)
{
    timers[timer].count = timers[timer].initValue;
}


// See if a system timer has expired
boolean timerExpired(int timer)
{
    if(timers[timer].count == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


