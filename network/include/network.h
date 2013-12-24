#include  "/usr/include/libnet.h"






int send_storm(libnet_t *lib_net,int size,int pcap_size);
u_int32_t Get_long_address(char *ip_address);
int Ethernet_Brodacast_Storm(u_char *enet_src,char *device,int storm_size);


