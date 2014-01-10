#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "arp.h"
#include "ethernet.h"
#include  "network.h"
#include  "comm.h"





extern LIST_ENTRY mission_list;
extern pthread_rwlock_t rwlock;
extern pthread_rwlock_t rwlock_env;
extern pthread_rwlock_t rwlock_run;
extern ENV *env;
extern RUNNING_MISSION *Running;


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
						a->test_time=cJSON_GetObjectItem(mission->param,"test_time")->valueint;
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

	int err;
	pthread_t ptemp;
	MISSION * run_misson;
	run_misson=(MISSION *)malloc(sizeof(MISSION));
	memset(run_misson,0,sizeof(MISSION));
	if(a->ALLGO)
		{

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
	else if((!a->ALLGO)&&a->type)
		{

		}
		

}

















