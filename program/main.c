#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "lib_ui.h"
#include "network.h"
#include "comm.h"


LIST_ENTRY mission_list;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwlock_env = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwlock_run = PTHREAD_RWLOCK_INITIALIZER;
ENV *env;
RUNNING_MISSION *Running;

void Init()
{
    InitializeListHead(&mission_list);
    env = (ENV *)malloc(sizeof(ENV));
    memset(env,0,sizeof(ENV));
	Running = (RUNNING_MISSION *)malloc(sizeof(RUNNING_MISSION));
	memset(Running,0,sizeof(RUNNING_MISSION));
}

void Uninit()
{
    pthread_rwlock_destroy(&rwlock);
    free(env);
	free(Running);
}

int main()
{ 
    Init();
    run_server();
    Uninit();
    return 0;
}
