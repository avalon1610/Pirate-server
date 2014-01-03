#define bool int
#define true 1
#define false 0
#include  "network.h"
#include  "../include/comm.h"
#include <time.h>
#include <pthread.h>




extern LIST_ENTRY mission_list;
extern pthread_rwlock_t rwlock;


int send_storm(libnet_t *lib_net,int size,int pcap_size)
{	int c;
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


void start_test()
{	

	struct ARP_Request_Storm_ARG a={
	.ip_dst="192.168.1.101",
	.ip_src="192.168.1.102",
	.enet_src={0x01,0x02,0x03,0x04,0x05,0x06},
	.enet_dst={0x01,0x02,0x03,0x04,0x05,0x06},
	.device="eth0",
	.storm_size=10000
	};
	
	LIST_ENTRY runnig_list;
	InitializeListHead(&runnig_list);
	pthread_t thread_id;
 	int return_id;
    pthread_rwlock_rdlock(&rwlock);
	LIST_ENTRY *current = mission_list.Flink;
	MISSION *entry;
	while(current != &mission_list)
	{
		entry = CONTAINING_RECORD(current,MISSION,node);
		if (entry->status==0 )
		{
		MISSION *run_mission = (MISSION *)malloc(sizeof(MISSION));
    	memset(run_mission,0,sizeof(MISSION));
		memcpy(run_mission,entry,sizeof(MISSION));
		InsertTailList(&runnig_list,&run_mission->node);
		}
		current = current->Flink;
	}
	pthread_rwlock_unlock(&rwlock);

	current = runnig_list.Flink;
	while(current != &runnig_list)
	{
		entry = CONTAINING_RECORD(current,MISSION,node);
		switch(entry->type){
		case ARP_REQUEST_STORM :
			pthread_create (&thread_id, NULL, &ARP_Request_Storm, (void *)&a); 
			pthread_join (thread_id, NULL);
			break;
		//case 


		}
		current = current->Flink;
		}
		
}












