#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "lib_ui.h"
#include "comm.h"
#include "network.h"


LIST_ENTRY mission_list;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwlock_env = PTHREAD_RWLOCK_INITIALIZER;
ENV *env;

void Init()
{
    InitializeListHead(&mission_list);
    env = (ENV *)malloc(sizeof(ENV));
    memset(env,0,sizeof(ENV));
}

void Uninit()
{
    pthread_rwlock_destroy(&rwlock);
    free(env);
}

int main()
{

    Init();
    run_server();
    Uninit();
    return 0;
}
