#include <stdio.h>
#include <pthread.h>
#include "lib_ui.h"
#include "comm.h"
#include "network.h"

LIST_ENTRY mission_list;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void Init()
{
    InitializeListHead(&mission_list);
}

void Uninit()
{
    pthread_rwlock_destroy(&rwlock);
}

int main()
{

    Init();
    run_server();
    Uninit();
    return 0;
}
