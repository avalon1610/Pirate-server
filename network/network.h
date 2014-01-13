#ifndef _NETWORK_H_
#define _NETWORK_H_
#include  "/usr/include/libnet.h"
#include "comm.h"


#define STORM_TIME 20

int send_storm(libnet_t *lib_net,int size,int pcap_size);
u_int32_t Get_long_address(u_char *ip_address);
void Create_Random(int len,u_char *Rd);
int icmp_storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size);
int send_storm_random_time(libnet_t *lib_net,int size,int pcap_size,int storm_time);
int send_storm_set_time(libnet_t *lib_net,int size,int pcap_size,int storm_time,int set_time);




#ifdef __cplusplus
extern "C" {
#endif
void Test_Work(/*COMMAND *a*/);

#ifdef __cplusplus
}
#endif

#endif


