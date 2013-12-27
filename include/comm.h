#ifndef COMM_H_
#define COMM_H_

#include "mylist.h"
#include "cJSON.h"

#define DEFAULT_STR_LEN 16
typedef enum _MISSION_TYPE
{
    ARP_REQUEST_STORM = 0,
    ARP_HOST_REPLY_STORM,
    ARP_CACHE_SATURATION_STORM,
    ARP_GRAMMEAR
} MISSION_TYPE;

typedef enum _MISSION_STATUS
{
    RUNNING = 0,
    PAUSE,
    STOP,
    ERROR
} MISSION_STATUS;

typedef struct _MISSION
{
    LIST_ENTRY node;
    char name[DEFAULT_STR_LEN];
    MISSION_TYPE type;
    MISSION_STATUS status;
    cJSON *param;
} MISSION,*PMISSION;

#endif
