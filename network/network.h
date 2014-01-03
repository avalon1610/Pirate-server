#ifndef _NETWORK_H_
#define _NETWORK_H_
#include  "/usr/include/libnet.h"
#include "arp.h"


int send_storm(libnet_t *lib_net,int size,int pcap_size);
u_int32_t Get_long_address(u_char *ip_address);


int Ethernet_Brodacast_Storm(u_char *enet_src,char *device,int storm_size);
int Ethernet_Grammar(u_char *enet_dst,u_char *enet_src,char *device);
int Ethernet_Fuzzer(u_char *enet_dst,u_char *enet_src,char *device);

void Create_Random(int len,u_char *Rd);
int icmp_storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size);



#ifdef __cplusplus
extern "C" {
#endif

void start_test();

#ifdef __cplusplus
}
#endif

#endif


