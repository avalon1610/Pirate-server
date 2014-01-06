#ifndef _ETHERNET_H_
#define _ETHERNET_H_


struct Ethernet_Brodacast_Storm
{
	
	u_char enet_src[6];
	char device[10];
	int storm_size;

};

struct Ethernet_Multicast_Storm
{
	u_char enet_src[6];
	char device[10];
	int storm_size;

};


struct Ethernet_Unicast_Storm
{
	u_char enet_dst[6];
	u_char enet_src[6];
	char device[10];
	int storm_size;


};

struct Ethernet_Grammar
{
	u_char enet_dst[6];
	u_char enet_src[6];
	char device[10];



};

struct Ethernet_Fuzzer
{
	u_char enet_dst[6];
	u_char enet_src[6];
	char device[10];


};


int Ethernet_Brodacast_Storm(struct Ethernet_Brodacast_Storm *a);
int Ethernet_Multicast_Storm(struct Ethernet_Multicast_Storm *a);
int Ethernet_Unicast_Storm(struct Ethernet_Unicast_Storm *a);
int Ethernet_Grammar(struct Ethernet_Grammar *a);
int Ethernet_Fuzzer(struct Ethernet_Fuzzer *a);


#endif
