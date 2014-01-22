#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <linux/limits.h>
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
    char exe_path[PATH_MAX] = {0};
    if (-1 == readlink("/proc/self/exe",exe_path,PATH_MAX))
    {
        puts("get execute path error!");
        return false;
    }
    char *pos = strrchr(exe_path,'/');
    if (pos == NULL)
    {
        puts("invalid exe path!");
        return false;
    }
    *(pos+1) = '\0';
    char config_path[PATH_MAX] = {0};
    sprintf(config_path,"%szlog.conf",exe_path);
    int rc = zlog_init(config_path);
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

int test_main()
{
	if (!Init())
		 return -1;

	ETHERNET_FUZZER a;
	//a=(ETHERNET_FUZZER *)malloc(sizeof(ETHERNET_FUZZER));
	memcpy(a.device,"eth0",5);
	u_char b[6]={0x00,0x01,0x02,0x03,0x04,0x05};
	u_char d[6]={0x02,0x01,0x02,0x03,0x04,0x05};
	memcpy(a.enet_dst,b,6);
	memcpy(a.enet_src,d,6);
	a.speed=10000.0;
	a.top_speed=false;
	Ethernet_Fuzzer(&a);


	

}
