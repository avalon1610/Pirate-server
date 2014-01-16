#ifndef _ETHERNET_H_
#define _ETHERNET_H_


typedef struct _ETHERNET_BRODACAST_STORM
{
	
	u_char enet_src[6];
	char device[10];
	int storm_size;
	int space_time;
}ETHERNET_BRODACAST_STORM,*PETHERNET_BRODACAST_STORM;

typedef struct _ETHERNET_MULTICAST_STORM
{
	u_char enet_src[6];
	char device[10];
	int storm_size;
	int space_time;
}ETHERNET_MULTICAST_STORM,*PETHERNET_MULTICAST_STORM;


typedef struct _ETHERNET_UNICAST_STORM
{
	u_char enet_dst[6];
	u_char enet_src[6];
	char device[10];
	int storm_size;
	int space_time;

}ETHERNET_UNICAST_STORM,*PETHERNET_UNICAST_STORM;

typedef struct _ETHERNET_GRAMMAR
{
	u_char enet_dst[6];
	u_char enet_src[6];
	char device[10];



}ETHERNET_GRAMMAR,*PETHERNET_GRAMMAR;

typedef struct _ETHERNET_FUZZER
{
	u_char enet_dst[6];
	u_char enet_src[6];
	char device[10];


}ETHERNET_FUZZER,*PETHERNET_FUZZER;


int Ethernet_Brodacast_Storm(ETHERNET_BRODACAST_STORM *a);
int Ethernet_Multicast_Storm(ETHERNET_MULTICAST_STORM *a);
int Ethernet_Unicast_Storm(ETHERNET_UNICAST_STORM *a);
int Ethernet_Grammar(ETHERNET_GRAMMAR *a);
int Ethernet_Fuzzer(ETHERNET_FUZZER *a);


#endif
