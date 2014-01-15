#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "zlog.h"
#include "ui.h"
#include "network.h"
#include "comm.h"


LIST_ENTRY mission_list;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwlock_env = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwlock_run = PTHREAD_RWLOCK_INITIALIZER;
ENV *env;
RUNNING_MISSION *Running;
zlog_category_t *c;

int Init()
{
    int rc = zlog_init("/etc/zlog.conf");
    if (rc)
    {
        printf("init zlog failed:%d\n",rc);
        return false;
    }
    c = zlog_get_category("my_cat");
    if (!c)
    {
        printf("get cat failed\n");
        zlog_fini();
        return false;
    }

    InitializeListHead(&mission_list);
    env = (ENV *)malloc(sizeof(ENV));
    if (env == NULL)
        return false;

    memset(env,0,sizeof(ENV));
	Running = (RUNNING_MISSION *)malloc(sizeof(RUNNING_MISSION));
    if (Running == NULL)
        return false;

	memset(Running,0,sizeof(RUNNING_MISSION));
    zlog_info(c,"System Init Success.\n");
    return true;
}

void Uninit()
{
    zlog_fini();
    pthread_rwlock_destroy(&rwlock);
    free(env);
	free(Running);
}

int main()
{ 
    if (!Init())
        return -1;
    run_server();
    Uninit();
    return 0;
}
