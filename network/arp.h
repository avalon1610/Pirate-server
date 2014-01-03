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

};


int ARP_Request_Storm(struct ARP_Request_Storm_ARG *a);
int APR_Host_Reply_Storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size);
int ARP_Grammear(u_char *ip_dst,u_char *enet_src,u_char *enet_dst,char *device);
int ARP_Cache_Saturation_Storm(u_char *ip_dst,u_char *enet_dst,char *device,int storm_size);

#endif
