
#include "timer.h"
#include "define.h"
#include <stdlib.h>

/* Miscellaneous timeval routines */

/**
 * Divide tvp by div, storing the result in tvp 
 */

typedef unsigned int uint32_t;



void
timerdiv_float(struct timeval *tvp, float div)
{
    double interval;

    if (div == 0 || div == 1)
        return;

    interval = ((double)tvp->tv_sec * 1000000 + tvp->tv_usec) / (double)div;
    tvp->tv_sec = interval / (int)1000000;
    tvp->tv_usec = interval - (tvp->tv_sec * 1000000);
}

/* Divide tvs by div, storing the result in tvs */
void timesdiv_float(struct timespec *tvs, float div)
{
    double interval;

    if (div == 0 || div == 1)
        return;

    interval = ((double)tvs->tv_sec * 1000000000 + tvs->tv_nsec) / (double)div;
    tvs->tv_sec = interval / (int)1000000000;
    tvs->tv_nsec = interval - (tvs->tv_sec * 1000000000);
}

void
timerdiv(struct timeval *tvp, COUNTER div)
{
  uint32_t interval;

    if (div == 0 || div == 1)
        return;

    interval = (uint32_t)tvp->tv_sec * 1000000 + tvp->tv_usec;
    do_div(interval, div);
    tvp->tv_sec = interval / 1000000;
    tvp->tv_usec = interval - (tvp->tv_sec * 1000000);
}

/* Divide tvs by div, storing the result in tvs */
void timesdiv(struct timespec *tvs, COUNTER div)
{
    uint32_t interval;
    
    if (div == 0 || div == 1)
        return;
        
    interval = (uint32_t)tvs->tv_sec * 1000000000 + tvs->tv_nsec;
    do_div(interval, div);
    tvs->tv_sec = interval / 1000000000;
    tvs->tv_nsec = interval - (tvs->tv_sec * 1000000000);
}

void
init_delta_time(delta_t *ctx)
{

    timerclear(ctx);

}

