#include "network.h"
#include "ethernet.h"
#include "zlog.h"

extern zlog_category_t *c;

int Ethernet_Brodacast_Storm(ETHERNET_BRODACAST_STORM *a)
{
	u_char *enet_src=a->enet_src;
	char *device=a->device;
	COUNTER speed= a->speed;
	int test_time=a->test_time;
	int storm_time=a->storm_time;
	bool top_speed=a->top_speed;
	
    libnet_t *l;
    libnet_ptag_t t;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	//u_char enet_src[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u_char enet_dst[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u_char payload[48] ={};
	memset(payload,0,48);
	
	l = libnet_init(
			  LIBNET_LINK_ADV,						  /* injection type */
			  device,								  /* network interface */
			  errbuf);								  /* errbuf */

	  if (l == NULL)
	  {
		  zlog_debug(c, "%s", errbuf);
		  return 0;;
	  }


		t = libnet_build_ethernet(
        				enet_dst,                        /* 目标主机的MAC地址 */
        				enet_src,                        /* 发送主机的MAC地址 */
        				0x0800,                        /* 以太网帧类型，这里是ARP */
        				payload,
        				48,
        				l,
        				0);							  /* libnet handle */
		  if (t == -1)
		  {
			  zlog_debug(c, "Can't build ethernet header: %s\n",
					  libnet_geterror(l));
			  return 0;

		  }

	
		if(storm_time)
		{
			send_storm_set_time(l,speed,test_time,storm_time,top_speed);
		}
		else
		{
			send_storm_random_time(l,speed,test_time,top_speed);
		}

		  
		 
return 1;

}



int Ethernet_Multicast_Storm(ETHERNET_MULTICAST_STORM *a)
{
	u_char *enet_src=a->enet_src;
	char *device=a->device;
	COUNTER speed= a->speed;
	int test_time=a->test_time;
	int storm_time=a->storm_time;
	bool top_speed=a->top_speed;

	int send_size=0;
   // u_int32_t i;
    libnet_t *l;
    libnet_ptag_t t;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	//int pcap_count=0;


	u_char enet_dst[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x01};
	u_char payload[46] ={};
	memset(payload,0,46);
	
	l = libnet_init(
			  LIBNET_LINK_ADV,						  /* injection type */
			  device,								  /* network interface */
			  errbuf);								  /* errbuf */

	  if (l == NULL)
	  {
		  zlog_debug(c, "%s", errbuf);
		  return 0;;
	  }

	
		 

		t = libnet_build_ethernet(
        				enet_dst,                        /* 目标主机的MAC地址 */
        				enet_src,                        /* 发送主机的MAC地址 */
        				0x0800,                        /* 以太网帧类型，这里是ARP */
        				payload,
        				46,
        				l,
        				0);							  /* libnet handle */
		  if (t == -1)
		  {
			  zlog_debug(c, "Can't build ethernet header: %s\n",
					  libnet_geterror(l));
			  return 0;

		  }
	
		
		if(storm_time)
			{
				send_storm_set_time(l,speed,test_time,storm_time,top_speed);
			}
			else
			{
				send_storm_random_time(l,speed,test_time,top_speed);
			}
	
	

return 1;
}


int Ethernet_Unicast_Storm(ETHERNET_UNICAST_STORM *a)
{
	u_char *enet_dst=a->enet_dst;
	u_char *enet_src=a->enet_src;
	char *device=a->device;
	COUNTER speed= a->speed;
	int test_time=a->test_time;
	int storm_time=a->storm_time;
	bool top_speed=a->top_speed;
	
	int send_size=0;

    libnet_t *l;
    libnet_ptag_t t;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	//int pcap_count=0;


	//u_char enet_dst[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x01};
	u_char payload[46] ={};
	memset(payload,0,46);
	
	l = libnet_init(
			  LIBNET_LINK_ADV,						  /* injection type */
			  device,								  /* network interface */
			  errbuf);								  /* errbuf */

	  if (l == NULL)
	  {
		  zlog_debug(c, "%s", errbuf);
		  return 0;;
	  }

	
		 

		t = libnet_build_ethernet(
        				enet_dst,                        /* 目标主机的MAC地址 */
        				enet_src,                        /* 发送主机的MAC地址 */
        				0x0800,                        /* 以太网帧类型，这里是ARP */
        				payload,
        				46,
        				l,
        				0);							  /* libnet handle */
		  if (t == -1)
		  {
			  zlog_debug(c, "Can't build ethernet header: %s\n",
					  libnet_geterror(l));
			  return 0;

		  }


		  if(storm_time)
			{
				send_storm_set_time(l,speed,test_time,storm_time,top_speed);
			}
			else
			{
				send_storm_random_time(l,speed,test_time,top_speed);
			}



 	return 1;
}


int Ethernet_Grammar(ETHERNET_GRAMMAR *a)
{
	u_char *enet_dst=a->enet_dst;
	u_char *enet_src=a->enet_src;
	char *device=a->device;
	COUNTER speed= a->speed;
	bool top_speed=a->top_speed;


	bool skip_timestamp = false;
	struct timeval last = { 0, 0 };
	delta_t delta_ctx;
	init_delta_time(&delta_ctx);
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	COUNTER bytes_sent=0;

	int send_len;
	int j;
	int k;
	int f;
	int send_size=0;
   // u_int32_t i;
    libnet_t *l;
    libnet_ptag_t t;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	u_char m_enet[12][6]={{0x11,0x22,0x33,0x44,0x55,0x66},{0xaa,0xbb,0xcc,0xdd,0xee,0xff},
	{0x01,0x00,0x5e,0x00,0x00,0x01},{0x01,0x00,0x5e,0x00,0x00,0x02},{0x01,0x00,0x5e,0x00,0x00,0x03},
	{0x01,0x00,0x5e,0x00,0x00,0x04},{0x01,0x00,0x5e,0x00,0x00,0x05},{0xff,0xff,0xff,0xff,0xff,0xff},
	{0xff,0xee,0xdd,0xcc,0xbb,0xaa},{0xd8,0x5d,0x4c,0x6e,0xc9,0x5e}};
	//int pcap_count=0;

	memcpy(m_enet[10],enet_dst,6);
	memcpy(m_enet[11],enet_src,6);

	u_int16_t protocol_type[18]={0x0000,0x0001,0x001f,0x007f,0x00fe,0x00ff,0x1fff,
		0x7fff,0xfff0,0xfffe,0xffff,0x86dd,0x060d,0x002d,0x002f,0x0806,0x002e,0x05dc};
	//u_char enet_dst[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x01};
	u_char payload[46] ={};
	memset(payload,0,46);
	
	l = libnet_init(
			  LIBNET_LINK_ADV,						  /* injection type */
			  device,								  /* network interface */
			  errbuf);								  /* errbuf */

	  if (l == NULL)
	  {
		  zlog_debug(c, "%s", errbuf);
		  return 0;;
	  }
	
	for(j=0;j<18;j++)
		for(k=0;k<12;k++)
			for(f=0;f<12;f++)
	{
		

		t = libnet_build_ethernet(
        				m_enet[k],                        /* 目标主机的MAC地址 */
        				m_enet[f],                        /* 发送主机的MAC地址 */
        				protocol_type[j],                        /* 以太网帧类型，这里是ARP */
        				payload,
        				46,
        				l,
        				0);							  /* libnet handle */
		  if (t == -1)
		  {
			  zlog_debug(c, "Can't build ethernet header: %s\n",
					  libnet_geterror(l));
			  return 0;

		  }


		  send_len = libnet_write(l);
		  	if (send_len == -1)
		    {
		        zlog_debug(c, "Write error: %s\n", libnet_geterror(l));
		    }
			if(!top_speed)
				{
					do_sleep(accurate_select,&delta_ctx,&start_time,speed,bytes_sent,send_len,&skip_timestamp);
					bytes_sent=bytes_sent+packet_s;
					if (!skip_timestamp)
					{
			                start_delta_time(&delta_ctx);
							
					}
				}

		  
		  libnet_clear_packet(l);
		  
	}
return 1;

}

int Ethernet_Fuzzer(ETHERNET_FUZZER *a)
{
	zlog_debug(c,"Start Ethernet_Fuzzer\n");
	bool skip_timestamp = false;
	struct timeval last = { 0, 0 };
	delta_t delta_ctx;
	init_delta_time(&delta_ctx);
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	COUNTER bytes_sent=0;

	
	u_char *enet_dst=a->enet_dst;
	u_char *enet_src=a->enet_src;
	char *device=a->device;
	bool top_speed=a->top_speed;
	COUNTER speed=a->speed;
	
	int send_len;
	int send_size=0;
	//u_int32_t i;
	libnet_t *l;
	libnet_ptag_t t;
	u_int8_t *packet;
	u_int32_t packet_s;
	int r;
	char errbuf[LIBNET_ERRBUF_SIZE];
	//int pcap_count=0;


	u_int16_t protocol_type=0x0000;
	
	l = libnet_init(
			  LIBNET_LINK_ADV,						  /* injection type */
			  device,								  /* network interface */
			  errbuf);								  /* errbuf */

	  if (l == NULL)
	  {
		  zlog_debug(c, "%s", errbuf);
		  return 0;;
	  }
	srand(time(NULL));
	
	while(protocol_type<=0xffff)
	{
		
		
	r=rand()%1000;
	
	if(r)
		{
		u_char *payload=(u_char *)malloc(r+1);

		
		
		Create_Random(r,payload);
		t = libnet_build_ethernet(
						enet_dst,						  /* 目标主机的MAC地址 */
						enet_src,						  /* 发送主机的MAC地址 */
						protocol_type,						 /* 以太网帧类型，这里是ARP */
						payload,
						r,
						l,
						0); 						  /* libnet handle */
		  if (t == -1)
		  {
			  zlog_debug(c, "Can't build ethernet header: %s\n",
					  libnet_geterror(l));
			  return 0;

		  }


		  	send_len = libnet_write(l);
			if (send_len == -1)
			{
				zlog_debug(c, "Write error: %s\n", libnet_geterror(l));
			}
			if(!top_speed)
				{
					
					do_sleep(accurate_select,&delta_ctx,&start_time,speed,bytes_sent,send_len,&skip_timestamp);
					bytes_sent=bytes_sent+send_len;
			        start_delta_time(&delta_ctx);
							
						
				}
		  libnet_clear_packet(l);
		  protocol_type=protocol_type+0x0001;
		  
		 free(payload);
		}
	}





return 1;

}


