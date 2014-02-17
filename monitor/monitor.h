#ifndef _MONITOR_H
#define _MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _MONITOR_TYPE
{
    M_PING = 0,
    M_ARP
} MONITOR_TYPE;

typedef enum _MONITOR_CTL_TYPE
{
    M_START = 0,
    M_STOP
} MONITOR_CTL_TYPE;

typedef enum _MONITOR_STATUS
{
    M_FINE = 0,
    M_ERROR
} MONITOR_STATUS;

MONITOR_STATUS monitor_ctl(MONITOR_CTL_TYPE,MONITOR_TYPE);

#ifdef __cplusplus
};
#endif

#endif
