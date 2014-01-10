#ifndef _ARP_H_
#define _ARP_H_


typedef struct _ARP_REQUEST_STORM
{

		u_char ip_dst[16];
		u_char ip_src[16];
		u_char enet_src[6];
		u_char enet_dst[6];
		char device[10];
		int storm_size;
		int test_time;
		int space_time;

}ARP_REQUEST_STORM,*PARP_REQUEST_STORM;

typedef struct _APR_HOST_REPLY_STORM
{
	u_char ip_dst[16];
	u_char ip_src[16];
	u_char enet_src[6];
	u_char enet_dst[6];
	char device[10];
	int storm_size;
	int test_time;
	int space_time;

}APR_HOST_REPLY_STORM,*PAPR_HOST_REPLY_STORM;


typedef struct _ARP_GRAMMEAR
{
	u_char ip_dst[16];
	u_char enet_src[6];
	u_char enet_dst[6];
	char device[10];


}ARP_GRAMMEAR,*PARP_GRAMMEAR;

typedef struct _ARP_CACHE_SATURATION_StORM
{
	u_char ip_dst[16];
	u_char enet_dst[6];
	char device[6];
	int storm_size;
	int test_time;
	int space_time;

}ARP_CACHE_SATURATION_STORM,*PARP_CACHE_SATURATION_STORM;

int ARP_Request_Storm(ARP_REQUEST_STORM *a);
int APR_Host_Reply_Storm(APR_HOST_REPLY_STORM *a);
int ARP_Grammear(ARP_GRAMMEAR *a);
int ARP_Cache_Saturation_Storm(ARP_CACHE_SATURATION_STORM *a);

#endif
