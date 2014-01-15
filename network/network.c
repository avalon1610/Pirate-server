#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "arp.h"
#include "ethernet.h"
#include "network.h"
#include "comm.h"
#include "zlog.h"
#include "timer.h"
#include "sleep.h"
#include "define.h"


#define USERDEF_TIMER 1


extern LIST_ENTRY mission_list;
extern pthread_rwlock_t rwlock;
extern pthread_rwlock_t rwlock_env;
extern pthread_rwlock_t rwlock_run;
extern ENV *env;
extern RUNNING_MISSION *Running;
extern zlog_category_t *c;



int send_storm_random_time(libnet_t *lib_net,int size,int pcap_size,int storm_time)
{
	int R;
	srand((unsigned)time(NULL));
	R=rand()%5;
	int l;
	clock_t s_e_t=0;
	clock_t start = clock();
    clock_t end = (clock() - start)/CLOCKS_PER_SEC;
	while(end<=storm_time)
			{	
				l=rand()%10;
				if((clock()-s_e_t)/CLOCKS_PER_SEC==R)
					{	
					send_storm(lib_net,size,pcap_size); 
					s_e_t=clock();
					}
					end = (clock() - start)/CLOCKS_PER_SEC;
				
					
			}
	return 1;
	
}

int send_storm_set_time(libnet_t *lib_net,int size,int pcap_size,int storm_time,int set_time)
{
	int l;
	clock_t s_e_t=0;
	clock_t start = clock();
    clock_t end = (clock() - start)/CLOCKS_PER_SEC;

	while(end <= storm_time)
			{	
			
				if((clock()-s_e_t)/CLOCKS_PER_SEC==set_time)
					{	
					
					send_storm(lib_net,size,pcap_size); 
					s_e_t=clock();
					}
			
					end = (clock() - start)/CLOCKS_PER_SEC;
				
					
			}

	return 1;
}


int send_storm(libnet_t *lib_net,int size,int pcap_size)
{	int c;
	clock_t start = clock();
    clock_t end = (clock() - start)/CLOCKS_PER_SEC;

	
		int send=0;
		while(send<size)
			{
				c =	libnet_write(lib_net);
				send=send+pcap_size;
				  if (c == -1)
	    		{
	       			 fprintf(stderr, "Write error: %s\n", libnet_geterror(lib_net));
	        			return -1;
	    		}
			}

		end = (clock() - start)/CLOCKS_PER_SEC;
	
	return 1;

}


u_int32_t Get_long_address(u_char *ip_address)
{

	int ip_len=16;
	u_int32_t changed_ip;
    unsigned int digit = 0;  // 记录ip地址4个整数
    int dotNum = 0;    // num of dot
    int digitNum = 0;  // num of digit
    char input = 0;
	int ipIdx;
	
	if(ip_address==NULL)
		return 0;
	for(ipIdx =0;ipIdx<ip_len;++ipIdx)
	{
		input = ip_address[ipIdx];
		if(input >= '0' && input <= '9') // 数字
		{
			++ digitNum;
			digit = 10 * digit + (input - '0');
			if(digit > 255) // 数字非法或长度过长
			{
				return 0;
			}
		}else if(input == '.')// 遇点，合并部分结果
		{
			++ dotNum;
			if(dotNum > digitNum)// 诸如 ..0.1 or 4..0.1
			{
				return 0;
			}else // 合并
			{
				changed_ip = (changed_ip<<8) | digit;
				digit = 0;
			}
		}else if(input == '\0') // 结束符，检查点数，返回结果
			{
				if(dotNum != 3)
					{
						return 0;
					}else
					{
						changed_ip = (changed_ip<<8) + digit;
						changed_ip= htonl(changed_ip);
						return changed_ip;
					}
			}else // 非法输入
				{
					return 0;
				}
	}
	
	return 0;

}



void Create_Random(int len,u_char *Rd)
{
	

	int i=len/4;
	int j;
	int R;
	srand((unsigned)time(NULL));
	for(j=0;j<i;j++)
		{
		
		R=rand();
		memcpy(Rd+4*j,&R,sizeof(int));

	}

	int m=len%4;
	R=rand();
	memcpy(Rd+4*j,&R,m);


}

void RUNING_MISSION_W(pthread_t thread_id,MISSION_STATUS status,clock_t start_time,MISSION_TYPE name)
{
	pthread_rwlock_wrlock(&rwlock_run);
	Running->running_thread_id=thread_id;
    Running->status=status;
	Running->mission_start_time=start_time;
	Running->type=name;
	pthread_rwlock_unlock(&rwlock_run);
}



void Test_MISS(MISSION *mission)
{
	zlog_debug(c,"Start Test_MISSION!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	pthread_t thread_id;
	if (mission->status==RUNNING)
		{
			switch(mission->type){
				case ARP_REQUEST_STORM_ :
					{
						ARP_REQUEST_STORM *a;
						a=(ARP_REQUEST_STORM *)malloc(sizeof(ARP_REQUEST_STORM));
						pthread_rwlock_wrlock(&rwlock_env);
						memcpy(a->ip_dst,env->target1,16);
						memcpy(a->ip_src,env->host,16);
						memcpy(a->enet_dst,env->target1_mac,6);
						memcpy(a->enet_src,env->host_mac,6);
						memcpy(a->device,env->device,6);
						pthread_rwlock_unlock(&rwlock_env); 
						a->storm_size=cJSON_GetObjectItem(mission->param,"storm_size")->valueint;
						a->space_time=cJSON_GetObjectItem(mission->param,"space_time")->valueint;
						pthread_create (&thread_id, NULL, (void *)ARP_Request_Storm, (void *)&a); 
						pthread_join (thread_id, NULL);
						break;
					}
			case ARP_CACHE_SATURATION_STORM_:
				//pthread_create (&thread_id, NULL, (void *)ARP_Cache_Saturation_Storm, (void *)&b); 
				//pthread_join (thread_id, NULL);
				break;
				  }



}
}



void Test_Work(COMMAND *a)
{
	zlog_debug(c,"Start Test_Work!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	int err;
	pthread_t ptemp;
	MISSION * run_misson;
	run_misson=(MISSION *)malloc(sizeof(MISSION));
	memset(run_misson,0,sizeof(MISSION));
	if(a->ALLGO)
		{
		zlog_debug(c,"Start ALL START!ORDER:%d,\n",a->order);
		if(a->order==START)
			{
			
			do{
				pthread_rwlock_rdlock(&rwlock);
			  	LIST_ENTRY *current = mission_list.Flink;
			  	MISSION *entry;
			  	while(current != &mission_list)
			  	{
				  entry = CONTAINING_RECORD(current,MISSION,node);
				  if(entry->status==RUNNING)
				  	{
						memcpy(run_misson,entry,sizeof(MISSION));
				  	}
				  current = current->Flink;
			  	}
			  	pthread_rwlock_unlock(&rwlock);
				Test_MISS(run_misson);
				}while(!run_misson->name);
			}
		}
	else 
		{
		zlog_debug(c,"Start ONE START!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		if(a->order==START)
			{
			
			do{
				pthread_rwlock_rdlock(&rwlock);
			  	LIST_ENTRY *current = mission_list.Flink;
			  	MISSION *entry;
			  	while(current != &mission_list)
			  	{
				  entry = CONTAINING_RECORD(current,MISSION,node);
				  if(entry->status==RUNNING)
				  	{
						memcpy(run_misson,entry,sizeof(MISSION));
				  	}
				  current = current->Flink;
			  	}
			  	pthread_rwlock_unlock(&rwlock);
				Test_MISS(run_misson);
				}while(!run_misson->name);
			}
		}
		

}




void
do_sleep(ACCURATE accurate ,delta_t *delta_ctx,struct timeval start_time,COUNTER speed,int send_size,int len ,bool *skip_timestamp)
{
	int userdef_timer=0;


    struct timespec adjuster = { 0, 0 };
    static struct timespec nap = { 0, 0 }, delta_time = {0, 0};
    struct timeval nap_for;
    struct timespec nap_this_time;
    static int32_t nsec_adjuster = -1, nsec_times = -1;
    static u_int32_t send = 0;      /* accellerator.   # of packets to send w/o sleeping */
    u_int64_t ppnsec; /* packets per nsec */
    static int first_time = 1;      /* need to track the first time through for the pps accelerator */
    static COUNTER skip_length = 0;


#ifdef USERDEF_TIMER
    adjuster.tv_nsec = userdef_timer * 1000;
#else
    adjuster.tv_nsec = 0;
#endif

    /*
     * this accelerator improves performance by avoiding expensive
     * time stamps during periods where we have fallen behind in our
     * sending
     */
    if (*skip_timestamp) {
        if ((COUNTER)len < skip_length) {
            skip_length -= len;
            return;
        }

        skip_length = 0;
        *skip_timestamp = false;
    }

    /* accelerator time? */
    if (send > 0) {
        send --;
        return;
    }


        if (timerisset(delta_ctx)) {
            COUNTER next_tx_us = (send_size + len) * 8 * 1000000;
            do_div(next_tx_us, speed);  /* bits divided by Mbps = microseconds */
            COUNTER tx_us = TIMEVAL_TO_MICROSEC(delta_ctx) - TIMEVAL_TO_MICROSEC(&start_time);
            COUNTER delta_us = (next_tx_us >= tx_us) ? next_tx_us - tx_us : 0;
            if (delta_us)
                /* have to sleep */
                NANOSEC_TO_TIMESPEC(delta_us * 1000, &nap);
            else {
                /*
                 * calculate how many bytes we are behind and don't bother
                 * time stamping until we have caught up
                 */
                timesclear(&nap);
                skip_length = (tx_us - next_tx_us) * speed;
                do_div(skip_length, 8 * 1000000);
                *skip_timestamp = true;
            }
        }
   

    /* 
     * since we apply the adjuster to the sleep time, we can't modify nap
     */
    nap_this_time.tv_sec = nap.tv_sec;
    nap_this_time.tv_nsec = nap.tv_nsec;

    zlog_debug(c, "packet size %d %d %d\t\tnap ", len, nap.tv_sec, nap.tv_nsec);



    if (nsec_adjuster < 0)
         nsec_adjuster = (nap_this_time.tv_nsec % 10000) / 1000;

          /* update in the range of 0-9 */
      nsec_times = (nsec_times + 1) % 10;

    	if (nsec_times < nsec_adjuster) {
                    /* sorta looks like a no-op, but gives us a nice round usec number */
             nap_this_time.tv_nsec = (nap_this_time.tv_nsec / 1000 * 1000) + 1000;
            } else {
              nap_this_time.tv_nsec -= (nap_this_time.tv_nsec % 1000);
            }
       zlog_debug(c, "(%d)\tnsec_times = %d\tnap adjust: %lu -> %lu", nsec_adjuster, nsec_times, nap.tv_nsec, nap_this_time.tv_nsec);            
       
    

    /* don't sleep if nap = {0, 0} */
    if (!timesisset(&nap_this_time))
        return;

    zlog_debug(c, "nap_time before delta calc: %d-%d " , nap_this_time.tv_sec, nap_this_time.tv_nsec);
    get_delta_time(delta_ctx, &delta_time);
    zlog_debug(c, "delta:    %d-%d                   " , delta_time.tv_sec, delta_time.tv_nsec);

    if (timesisset(&delta_time)) {
        if (timescmp(&nap_this_time, &delta_time, >)) {
            timessub(&nap_this_time, &delta_time, &nap_this_time);
            zlog_debug(c, "timesub: %lu %lu", delta_time.tv_sec, delta_time.tv_nsec);
        } else { 
            timesclear(&nap_this_time);
            zlog_debug(c, "timesclear:%d %d " , delta_time.tv_sec, delta_time.tv_nsec);
        }
    }

    /* apply the adjuster... */
    if (timesisset(&adjuster)) {
        if (timescmp(&nap_this_time, &adjuster, >)) {
            timessub(&nap_this_time, &adjuster, &nap_this_time);
        } else { 
            timesclear(&nap_this_time);
        }
    }

    zlog_debug(c, "Sleeping:     %d-%d              " , nap_this_time.tv_sec, nap_this_time.tv_nsec);

    /*
     * Depending on the accurate method & packet rate computation method
     * We have multiple methods of sleeping, pick the right one...
     */
    switch (accurate) {
    case accurate_select:
        select_sleep(nap_this_time);
        break;

    case accurate_ioport:
        ioport_sleep(nap_this_time);
        break;

    case accurate_gtod:
        gettimeofday_sleep(nap_this_time);
        break;

    case accurate_nanosleep:
        nanosleep_sleep(nap_this_time);
        break;

    default:
        zlog_debug(c, "Unknown timer mode %d", accurate);
    }


    zlog_debug(c, "sleep delta:%d-%d " , delta_time.tv_sec, delta_time.tv_nsec);


}















