#ifndef COMM_H_
#define COMM_H_

#include "mylist.h"

#define DEFAULT_STR_LEN 16
enum MISSION_TYPE
{
    ARP_REQUEST_STORM = 0,
    ARP_HOST_REPLY_STORM,
    ARP_CACHE_SATURATION_STORM,
    ARP_GRAMMEAR
};

typedef struct _MISSION
{
    LIST_ENTRY node;
    char name[DEFAULT_STR_LEN];
    MISSION_TYPE type;

} MISSION,*PMISSION;

#endif
