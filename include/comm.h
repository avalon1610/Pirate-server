#ifndef COMM_H_
#define COMM_H_

#include "mylist.h"
#include "cJSON.h"

#define DEFAULT_STR_LEN 16
#ifdef __cplusplus
extern "C" {
#endif

typedef enum _MISSION_TYPE
{
    ARP_REQUEST_STORM = 0,
    ARP_HOST_REPLY_STORM = 1,
    ARP_CACHE_SATURATION_STORM = 2,
    ARP_GRAMMEAR
} MISSION_TYPE;

typedef enum _MISSION_STATUS
{
    RUNNING = 0,
    PAUSE,
    STOP,
    ERROR,
    END
} MISSION_STATUS;

typedef enum _SCAN_STATUS
{
    RUNNING = 0,
    PAUSE,
    STOP,
    ERROR
} SCAN_STATUS;

typedef struct _MISSION
{
    LIST_ENTRY node;
    char name[DEFAULT_STR_LEN];
    MISSION_TYPE type;
    MISSION_STATUS status;
    cJSON *param;
} MISSION,*PMISSION;

typedef struct _ENV {
    unsigned char host[16];
    unsigned char target1[16];
    unsigned char target2[16];
    unsigned char host_mac[6];
    unsigned char target1_mac[6];
    unsigned char target2_mac[6];
} ENV,*PENV; 

typedef enum _COMMAND_TYPE
{
    SCAN = 0,
    MISSION,
    ANALYIZE
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
    }
    bool ALL; 
} COMMAND,*PCOMMAND;

typedef struct _RUNNING_MISSION{
	pthread_t running_thread_id;
	int mission_start_time;
	MISSION_STATUS status;
	char miss_name[10];
}RUNNING_MISSION,*RUNNING_MISSION;

int DbgPrint(const char *format,...);
int command_control(COMMAND cmd);

#ifdef __cplusplus
};
#endif // c++

#endif

