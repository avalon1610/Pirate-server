
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/io.h>
#include "define.h"
#include "timer.h"
float gettimeofday_sleep_value;
int ioport_sleep_value;


void 
ioport_sleep_init(void)
{
    ioperm(0x80,1,1);
    ioport_sleep_value = inb(0x80);

}

void 
ioport_sleep(const struct timespec nap)
{
    u_int32_t usec;
    struct timeval nap_for;
    time_t i;

    TIMESPEC_TO_TIMEVAL(&nap_for, &nap);

    /* 
     * process the seconds, we do this in a loop so we don't have to 
     * use slower 64bit integers or worry about integer overflows.
     */
    for (i = 0; i < nap_for.tv_sec; i ++) {
        usec = SEC_TO_MICROSEC(nap_for.tv_sec);
        while (usec > 0) {
            usec --;
            outb(ioport_sleep_value, 0x80);
        }
    }

    /* process the usec */
    usec = nap.tv_nsec / 1000;
    usec --; /* fudge factor for all the above */
    while (usec > 0) {
        usec --;
        outb(ioport_sleep_value, 0x80);
    }

    usec = nap.tv_nsec;

}
