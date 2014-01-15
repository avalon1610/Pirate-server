
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>     
#include <errno.h>
#include <string.h>
#include <sys/event.h>
#include <sys/io.h>



#ifndef __SLEEP_H__
#define __SLEEP_H__

static inline void
nanosleep_sleep(struct timespec nap)
{
    nanosleep(&nap, NULL);
}


/*
 * Straight forward... keep calling gettimeofday() unti the apporpriate amount
 * of time has passed.  Pretty damn accurate from 1 to 100Mbps
 */
static inline void
gettimeofday_sleep(struct timespec nap)
{
    struct timeval now, sleep_until, nap_for;
    gettimeofday(&now, NULL);
    TIMESPEC_TO_TIMEVAL(&nap_for, &nap);
    timeradd(&now, &nap_for, &sleep_until);
    
    do {
        gettimeofday(&now, NULL);
    } while (timercmp(&now, &sleep_until, <));
}


#ifdef HAVE_ABSOLUTE_TIME
#include <CoreServices/CoreServices.h>

/* 
 * Apple's AbsoluteTime functions give at least .1usec precision
 * which is pretty damn sweet
 */
static inline void
absolute_time_sleep(struct timespec nap)
{
    AbsoluteTime sleep_until, naptime, time_left;
    Nanoseconds nanosec;

    nanosec = UInt64ToUnsignedWide(TIMESPEC_TO_NANOSEC(&nap));
    naptime = NanosecondsToAbsolute(nanosec);

    sleep_until = AddAbsoluteToAbsolute(UpTime(), naptime);

    do {
        time_left = SubAbsoluteFromAbsolute(sleep_until, UpTime());
    } while (NonZero(time_left));
}

#endif /* HAVE_ABSOLUTE_TIME */




/* 
 * sleep for some time using the select() call timeout method.   This is 
 * highly portable for sub-second sleeping, but only for about 1msec
 * resolution which is pretty much useless for our needs.  Keeping it here
 * for furture reference
 */
void 
select_sleep(const struct timespec nap)
{
    struct timeval timeout;

    TIMESPEC_TO_TIMEVAL(&timeout, &nap);

    if (select(0, NULL, NULL, NULL, &timeout) < 0)
        warnx("select_sleep() returned early due to error: %s", strerror(errno));
}


/*
 * ioport_sleep() only works on Intel and quite possibly only Linux.
 * But the basic idea is to write to the IO Port 0x80 which should
 * take exactly 1usec regardless of the CPU speed and without 
 * calling a sleep method which allows the kernel to service another thread
 * Idea stolen from: http://c-faq.com/osdep/sd25.html
 */
extern int ioport_sleep_value;

/* before calling port_sleep(), you have to call port_sleep_init() */
void ioport_sleep_init(void);

void ioport_sleep(const struct timespec nap);

#endif /* __SLEEP_H__ */
