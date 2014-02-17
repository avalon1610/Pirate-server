#ifndef COMM_H_
#define COMM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mylist.h"
#include "cJSON.h"

#define DEFAULT_STR_LEN 16
#define IP_LEN 16
#define MAC_LEN 6
#ifdef __cplusplus
extern "C" {
#endif

static inline void *NEW(size_t s)
{
    void *p = malloc(s);
    memset(p,0,s);
    return p;
}

static inline void DELETE(void *p)
{
    free(p);
}

typedef enum _MISSION_TYPE
{
    ARP_REQUEST_STORM_ = 0,
    ARP_HOST_REPLY_STORM_ = 1,
    ARP_CACHE_SATURATION_STORM_ = 2,
    ARP_GRAMMEAR_=3,
    ICMP_STORM_STORM_=4,
    ICMP_FUZZER_=5,
    ICMP_GRAMMAR_=6,
    ICMP_TYPE_CODE_=7
} MISSION_TYPE;

typedef enum _MISSION_STATUS
{
    STOPPED = 0,
    RUNNING,
    PAUSED,
    ERROR,
    END
} MISSION_STATUS;

typedef enum _SCAN_STATUS
{
    S_RUNNING = 0,
    S_PAUSED,
    S_STOPPED,
    S_ERROR
} SCAN_STATUS;

typedef enum _SCAN_TYPE
{
    TCP_SCAN,
    UDP_SCAN
} SCAN_TYPE;

typedef struct _MISSION
{
    LIST_ENTRY node;
    char name[DEFAULT_STR_LEN];
    MISSION_TYPE type;
    MISSION_STATUS status;
    cJSON *param;
} MISSION,*PMISSION;

typedef struct _ENV {
    unsigned char host[IP_LEN];
    unsigned char target1[IP_LEN];
    unsigned char target2[IP_LEN];
    unsigned char host_mac[MAC_LEN];
    unsigned char target1_mac[MAC_LEN];
    unsigned char target2_mac[MAC_LEN];
	unsigned char device[DEFAULT_STR_LEN];
} ENV,*PENV;

typedef enum _COMMAND_TYPE
{
    T_SCAN = 0,
    T_MISSION,
    T_ANALYIZE
} COMMAND_TYPE;

typedef enum _COMMAND_ORDER
{
    START = 0,
    PAUSE,
    RESUME,
    STOP
} COMMAND_ORDER;

typedef struct _COMMAND
{
    COMMAND_TYPE type;
    COMMAND_ORDER order;
    union
    {
        MISSION_TYPE m_type;
        SCAN_TYPE s_type;
    };
    bool ALLGO; 
} COMMAND,*PCOMMAND;

typedef struct _RUNNING_MISSION{
	pthread_t running_thread_id;
	int mission_start_time;
	MISSION_STATUS status;
	MISSION_TYPE type;
}RUNNING_MISSION,*PRUNNING_MISSION;

typedef enum {
    accurate_gtod = 0,
    accurate_select = 1,
    accurate_rdtsc = 2,
    accurate_ioport = 3,
    accurate_nanosleep = 4,
    accurate_abs_time = 5
} ACCURATE;



int DbgPrint(const char *format,...);

#ifdef __cplusplus
};
#endif // c++

#endif

