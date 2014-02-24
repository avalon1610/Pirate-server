#include "monitor.h"
#include "comm.h"
#include "zlog.h"
#include "ping.h"
#include <errno.h>
#include <signal.h>

extern pthread_rwlock_t rwlock;
extern pthread_rwlock_t rwlock_env;
extern LIST_ENTRY mission_list;
extern ENV *env;
extern zlog_category_t *c;

static int check_mission_queue()
{
    int count = 0;
    MISSION *entry;
    LIST_ENTRY *current = mission_list.Flink;
    pthread_rwlock_rdlock(&rwlock);
    while (current != &mission_list)
    {
        entry = CONTAINING_RECORD(current,MISSION,node);
        if (entry->status == RUNNING)
            count++;
        current = current->Flink;
    }
    pthread_rwlock_unlock(&rwlock);

    zlog_debug(c,"mission queue count:%d\n",count);
    return count;
}

static pthread_t ping_tid = 0;
static int check_monitor_running(MONITOR_TYPE t)
{
    // check
    bool running = false;
    if (ping_tid != 0 && t == M_PING)
        if (pthread_kill(ping_tid, 0) != ESRCH)
            running = true;
    zlog_debug(c,"checking montior type[%d] running...[%s]\n",t,running?"Yes":"No");
    return running;
}

void ping_thread(void *data)
{
    const int argc = 2;
    char argv[IP_LEN] = {0};
    char *target = data;

    strcpy(argv,target);
    zlog_debug(c,"get target:%s[%08X]\n",target,(int)target);
    free(target);
    ping_main(argc,argv);
}


static MONITOR_STATUS start_ping(const char *target)
{
    const int argc = 2;
    char *t = (char *)malloc(IP_LEN);

    if (target == NULL)
        return M_ERROR;
    memset(t,0,IP_LEN);
    memcpy(t,target,IP_LEN);

    zlog_debug(c,"start ping monitor on %s[%08X]\n",t,(int)t);
    pthread_create(&ping_tid,NULL,(void *)ping_thread,(void *)t);
}

static MONITOR_STATUS stop_ping()
{
    int ret ;
    ret = pthread_kill(ping_tid,SIGINT);
    if (ret != 0)
    {
        zlog_error(c,"kill ping thread [%d] failed:%d\n",ping_tid,ret);
        return M_ERROR;
    }
    zlog_info(c,"ping monitor thread [%d] exit.\n",ping_tid);
    return M_FINE;
}

static MONITOR_STATUS start_arp()
{
}

static MONITOR_STATUS stop_arp()
{
}

MONITOR_STATUS monitor_ctrl(MONITOR_CTL_TYPE ct,MONITOR_TYPE t)
{
    if (ct == M_START)
    {
        if (check_monitor_running(t))
            return M_FINE;
        else
        {
            // start monitor
            char target[IP_LEN] = {0};
            pthread_rwlock_rdlock(&rwlock_env);
            memcpy(target,env->target1,IP_LEN);
            pthread_rwlock_unlock(&rwlock_env);
            if (t == M_PING)
                return start_ping(target);
            else if (t == M_ARP)
                return start_arp();
        }
    }
    else if (ct == M_STOP)
    {
        if (check_monitor_running(t))
        {
            if (check_mission_queue() > 0)
            {
                // there are more mission need monitor
                // do nothing
                return M_FINE;
            }
            else
            {
                // stop monitor
                if (t == M_PING)
                    return stop_ping();
                else if (t == M_ARP)
                    return stop_arp();
            }
        }
    }

    return M_ERROR;
}
