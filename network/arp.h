#ifndef _ARP_H_
#define _ARP_H_


struct ARP_Request_Storm_ARG
{

		u_char ip_dst[16];
		u_char ip_src[16];
		u_char enet_src[6];
		u_char enet_dst[6];
		char device[10];
		int storm_size;
		int test_time;
		int space_time;

};

struct APR_Host_Reply_Storm
{
	u_char ip_dst[16];
	u_char ip_src[16];
	u_char enet_src[6];
	u_char enet_dst[6];
	char device[10];
	int storm_size;
	int test_time;
	int space_time;

};


struct ARP_Grammear
{
	u_char ip_dst[16];
	u_char enet_src[6];
	u_char enet_dst[6];
	char device[10];


};

struct ARP_Cache_Saturation_Storm
{
	u_char ip_dst[16];
	u_char enet_dst[6];
	char device[6];
	int storm_size;
	int test_time;
	int space_time;

};

int ARP_Request_Storm(struct ARP_Request_Storm_ARG *a);
int APR_Host_Reply_Storm(struct APR_Host_Reply_Storm *a);
int ARP_Grammear(struct ARP_Grammear *a);
int ARP_Cache_Saturation_Storm(struct ARP_Cache_Saturation_Storm *a);

#endif
