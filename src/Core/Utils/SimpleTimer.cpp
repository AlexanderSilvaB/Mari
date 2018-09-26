#include "SimpleTimer.h"
#include <unistd.h>

using namespace std;

SimpleTimer::SimpleTimer()
{
    Restart();
}

void SimpleTimer::Restart()
{
    clock_gettime(CLOCK_REALTIME, &start);
}

/* return elapsed time in seconds */
float SimpleTimer::Seconds() 
{
    return Millis() / 1000.0f;
}

uint32_t SimpleTimer::Millis() 
{
    return Micros() / 1000.0f;
}

uint32_t SimpleTimer::Micros() 
{
    clock_gettime(CLOCK_REALTIME, &end);
    float value = ((end.tv_sec - start.tv_sec) * 1000000.0f + (end.tv_nsec - start.tv_nsec) / 1000.0f);
    return value;
}

/* return estimated maximum value for elapsed() */
float SimpleTimer::SecondsMax() 
{
    /* Portability warning: elapsed_max() may return too high a value on systems
    * where std::clock_t overflows or resets at surprising values.
    */
    return Seconds();
}

/* return minimum value for elapsed() */
float SimpleTimer::SecondsMin() 
{
    return Seconds();
}

void SimpleTimer::Wait(int ms)
{
    usleep( 1000 * ms );
}