#include "network.h"
#include "arp.h"
#include "comm.h"
#include "zlog.h"

#define MTU 1500


extern zlog_category_t *c;
void frag_and_send(u_int8_t *payload, u_int32_t total_pload_size,libnet_t *l);
u_int16_t get_sum(u_int8_t *payload, u_int32_t total_pload_size, \
                u_int16_t id, u_int16_t seq);



int Icmp_storm(ICMP_STORM_STORM *a )
{


	zlog_debug(c,"Start icmp_storm\n");
	RUNING_MISSION_W(pthread_self(),RUNNING,clock(),ICMP_STORM_STORM_);

	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->ip_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	int test_time=a->test_time;
	int storm_time=a->storm_time;
	bool top_speed=a->top_speed;
	COUNTER speed=a->speed;
	printf("ip_dst:%s  ip_src:%s\n",ip_dst,ip_src);

   
	u_int32_t d,s;
    libnet_t *l = NULL;
    u_long src_ip, dst_ip ;
    u_long count = 10;
    libnet_ptag_t t;
    char *payload = NULL;
    u_short payload_s = 0;
	u_int8_t *packet;
    u_int32_t packet_s;
    char *pDst = NULL, *pSrc = NULL;
    char errbuf[LIBNET_ERRBUF_SIZE];
    char label[LIBNET_LABEL_SIZE];
	u_int8_t icmp_type=0x08;
   	u_int8_t icmp_payload[20]={'a','b','c','d','e','f','g','h','i','j','k',
							'l','m','n','o','p','q','r','s','t'};
 
	d=Get_long_address(ip_dst);
	s=Get_long_address(ip_src);
    l = libnet_init(
                LIBNET_LINK_ADV,                  /* injection type */
                device,                       /* network interface */
                errbuf);                      /* errbuf */

      if (l == NULL)
        {
            /* we should run through the queue and free any stragglers */
            zlog_debug(c, "libnet_init() failed: %s", errbuf);
            exit(EXIT_FAILURE);
        }

	  
		t = libnet_build_icmpv4_echo(
				ICMP_ECHO,							  /* type */
				0,									  /* code */
				0,									  /* checksum */
				0x42,								  /* id */
				0x42,								  /* sequence number */
				icmp_payload,								  /* payload */
				20,									  /* payload size */
				l,									  /* libnet handle */
				0);
			if (t == -1)
			{
				zlog_debug(c, "Can't build ICMP header: %s\n",
						libnet_geterror(l));
			}
	
		t = libnet_build_ipv4(
				LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_s, /* length */
				0,									  /* TOS */
				0x42,								  /* IP ID */
				0,									  /* IP Frag */
				64, 								  /* TTL */
				IPPROTO_ICMP,						  /* protocol */
				0,									  /* checksum */
				s, 							  /* source IP */
				d, 							  /* destination IP */
				payload,							  /* payload */
				payload_s,							  /* payload size */
				l,									  /* libnet handle */
				0);
			if (t == -1)
			{
				zlog_debug(c, "Can't build IP header: %s\n", libnet_geterror(l));

			}

		t = libnet_build_ethernet(
						   enet_dst,						/* 目标主机的MAC地址 */
						   enet_src,						/* 发送主机的MAC地址 */
						   0x0800,						 
						   NULL,
						   0,
						   l,
						   0);
		if (t == -1)
					{
						zlog_debug(c, "Can't build ethernet header: %s\n",
								libnet_geterror(l));
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


int Icmp_Fuzzer(ICMP_FUZZER *a)
{
	zlog_debug(c,"Start Icmp_Fuzzer\n");
	RUNING_MISSION_W(pthread_self(),RUNNING,clock(),ICMP_STORM_STORM_);
	
	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->ip_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	bool top_speed=a->top_speed;
	COUNTER speed=a->speed;

	u_int32_t d,s;
    libnet_t *l = NULL;
    libnet_ptag_t t;
	int r;
	u_int16_t Icmp_type_code=0x0000;
	int send_len;
	bool skip_timestamp = false;
	struct timeval last = { 0, 0 };
	delta_t delta_ctx;
	init_delta_time(&delta_ctx);
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	COUNTER bytes_sent=0;
	char errbuf[LIBNET_ERRBUF_SIZE];
	d=Get_long_address(ip_dst);
	s=Get_long_address(ip_src);
	u_int16_t id, seq;
	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);
	seq=1;
	
	l = libnet_init(
                LIBNET_LINK_ADV,                  /* injection type */
                device,                       /* network interface */
                errbuf);                      /* errbuf */

    if (l == NULL)
        {
            /* we should run through the queue and free any stragglers */
            zlog_debug(c, "libnet_init() failed: %s", errbuf);
            return 0;
        }

	srand(time(NULL));

while(Icmp_type_code <=0xffff)
{
		
	r=rand()%1000;
	if(r)
		{
		u_int8_t *payload=(u_char *)malloc(r);
		Create_Random(r,payload);
		
	
		u_int8_t Icmp_type=(u_int8_t)(Icmp_type_code>>8);
		u_int8_t Icmp_code=(u_int8_t)(Icmp_type_code);
		//u_char *payload=(u_char *)malloc(r+1);

		//Create_Random(r,payload);

		
		if (libnet_build_icmpv4_echo(Icmp_type, Icmp_code, 0, id, seq,\
        (u_int8_t*)payload,sizeof(payload), l, 0) == -1)
				  {
				    zlog_debug(c, "Error building ICMP header: %s\n",\
				        libnet_geterror(l));
				    libnet_destroy(l);
				    return 0;
				  }
		
		t = libnet_build_ipv4(
					LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + sizeof(payload), /* length */
					0,									  /* TOS */
					0x42,								  /* IP ID */
					0,									  /* IP Frag */
					64, 								  /* TTL */
					IPPROTO_ICMP,						  /* protocol */
					0,									  /* checksum */
					s, 							  /* source IP */
					d, 							  /* destination IP */
					NULL,							  /* payload */
					0,							  /* payload size */
					l,									  /* libnet handle */
					0);
		if (t == -1)
				{
					zlog_debug(c, "Can't build IP header: %s\n", libnet_geterror(l));
					return 0;

				}

		t = libnet_build_ethernet(
							   enet_dst,						/* 目标主机的MAC地址 */
							   enet_src,						/* 发送主机的MAC地址 */
							   0x0800,						 
							   NULL,
							   0,
							   l,
							   0);
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
		  Icmp_type_code=Icmp_type_code+0x0001;
		  
		 free(payload);
		}
		

		
	}
}


int Icmp_Grammar(ICMP_GRAMMAR *a)
{
	zlog_debug(c,"Start Icmp_Grammar\n");
	RUNING_MISSION_W(pthread_self(),RUNNING,clock(),ICMP_GRAMMAR_);


    /* get the arg*/	
	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->ip_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	bool top_speed=a->top_speed;
	COUNTER speed=a->speed;

	/*SPEED CONTROL  ARG*/
	int send_len;
	bool skip_timestamp = false;
	struct timeval last = { 0, 0 };
	delta_t delta_ctx;
	init_delta_time(&delta_ctx);
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	COUNTER bytes_sent=0;


	u_int32_t d,s;
    libnet_t *l = NULL;
	int r;
	int j=0;
	int	i=0;
	u_int8_t Icmp_code=0x00;
	char errbuf[LIBNET_ERRBUF_SIZE];
	u_int8_t *payload_use;
	int l_use;
	u_int8_t payload[]="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	int p_l_1=sizeof(payload);
	
	u_int8_t head_type	= ICMP_ECHO;
	bool type_add;
	u_int16_t Icmp_id;
	u_int16_t Icmp_seq;


	d=Get_long_address(ip_dst);
	s=Get_long_address(ip_src);
	l = libnet_init(
			LIBNET_LINK_ADV,				  /* injection type */
			device, 					  /* network interface */
			errbuf);					  /* errbuf */

  	if (l == NULL)
		{
			/* we should run through the queue and free any stragglers */
			zlog_debug(c, "libnet_init() failed: %s", errbuf);
			exit(EXIT_FAILURE);
		}


	u_int16_t id[7]={0x0000,0xffff,0x0011,0x1fff,0x7fff,0xfffe,0xff00};
	u_int32_t timestamp[5]={0x00000000,0x11111111,0xffffffff,0x7fffffff,0xfffffffe};
	u_int8_t *long_payload=(u_int8_t *)malloc(983);
	memset(long_payload,0x2a,983);
	int p_l_2=983;
	
	
	int m,n,k;
	while(Icmp_code <=0xff)
	{
		switch(head_type)
				{
			case ICMP_ECHO:
				
						if(i<7)
						{
							payload_use = payload;
							Icmp_id=id[i];
							if(j<7)
								{
									Icmp_seq=id[j];
									j++;
									payload_use = payload;
									l_use = p_l_1;
									
								}
							if(j==7)
								{
								payload_use = long_payload; 
								l_use=p_l_2;
								
								i++;
							
								}
							
							type_add =false;
							}
						else
							{
								type_add=true;
								i=0;
								j=0;
							}

						
						if (libnet_build_icmpv4_echo(ICMP_ECHO, Icmp_code, 0, Icmp_id, Icmp_seq,\
				        (u_int8_t*)payload_use,l_use, l, 0) == -1)
								  {
								    zlog_debug(c, "Error building ICMP echo header: %s\n",\
								        libnet_geterror(l));
								    libnet_destroy(l);
								    return 0;
								  }
						break;
				
			case ICMP_UNREACH:
						if (libnet_build_icmpv4_unreach(ICMP_UNREACH,Icmp_code,0,payload,sizeof(payload),l,0)==-1)
						{
						   zlog_debug(c, "Error building ICMP unreach header: %s\n",\
								        libnet_geterror(l));
								    libnet_destroy(l);
								    return 0;


						}
						break;

			case ICMP_TSTAMP:

					if(i<7)
						{
							Icmp_id=id[i];
							if(j<7)
								{
									Icmp_seq=id[j];
									j++;
								}
							if(j==7)
								{
								i++;
				
								}
							
							type_add =false;
							m=j%5;
							n=j%5;
							k=(i+j)%5;
							}
						else
							{
								type_add=true;
								i=0;
								j=0;
							}
				   if(libnet_build_icmpv4_timestamp(ICMP_TSTAMP,Icmp_code,0,Icmp_id,Icmp_seq,timestamp[m],timestamp[n],timestamp[k],NULL,0,l,0)==-1)
				   	{
					   zlog_debug(c, "Error building ICMP timestamp header: %s\n",\
											libnet_geterror(l));
						libnet_destroy(l);
						return 0;


				   	}
				   break;
			case ICMP_TIMXCEED:
				   if(libnet_build_icmpv4_timeexceed(ICMP_TIMXCEED,Icmp_code,0,NULL, 0, l,0)==-1)
				   	{

					   zlog_debug(c, "Error building ICMP timeexceed header: %s\n",\
											libnet_geterror(l));
						libnet_destroy(l);
						return 0;

				   	}
				   break;
			case ICMP_REDIRECT:
			   if( libnet_build_icmpv4_redirect(ICMP_REDIRECT,Icmp_code,0,0,NULL,0,l,0)==-1)
				   	{

					   zlog_debug(c, "Error building ICMP redirect header: %s\n",\
											libnet_geterror(l));
						libnet_destroy(l);
						return 0;

				   	}
			   			break;
			
			default:
				break;
			}

			
			if( libnet_build_ipv4(
						LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + sizeof(payload), \
						0,0x42,0,64,IPPROTO_ICMP,0,s,d,NULL,0,l,0)==-1)
					{
						zlog_debug(c, "Can't build IP header: %s\n", libnet_geterror(l));
						return 0;

					}

			if(libnet_build_ethernet(enet_dst,enet_src,0x0800,NULL,0,l,0)==-1)
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
			  if(Icmp_code==0xff )
			  	{
			  	type_add=true;
			  	if(head_type == ICMP_ECHO)
					{
						head_type=ICMP_UNREACH;
						Icmp_code=0x00;
						

					}
				else if(head_type ==ICMP_UNREACH )
					{
						head_type=ICMP_TSTAMP;
						Icmp_code=0x00;
						

					}
				else if(head_type ==ICMP_TSTAMP )
					{
						head_type=ICMP_TIMXCEED;
						Icmp_code=0x00;
						

					}
				else if(head_type ==ICMP_TIMXCEED )
					{
						head_type=ICMP_REDIRECT;
						Icmp_code=0x00;
						

					}
				else
					break;
			  	}
				else if(type_add)
					{
			  			Icmp_code=Icmp_code+0x01;
					}

					
		}
			  
		
}


int Icmp_Type_Code_Cross_Product(ICMP_TYPE_CODE *a)
{

	zlog_debug(c,"Start Icmp_Grammar\n");
	RUNING_MISSION_W(pthread_self(),RUNNING,clock(),ICMP_TYPE_CODE_);


	/* get the arg*/	
	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->ip_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	bool top_speed=a->top_speed;
	COUNTER speed=a->speed;

	/*SPEED CONTROL  ARG*/
	int send_len;
	bool skip_timestamp = false;
	struct timeval last = { 0, 0 };
	delta_t delta_ctx;
	init_delta_time(&delta_ctx);
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	COUNTER bytes_sent=0;


	u_int32_t d,s;
	libnet_t *l = NULL;
	int r;
	int j=0;
	int i=0;
	u_int8_t Icmp_code=0x00;
	char errbuf[LIBNET_ERRBUF_SIZE];



	d=Get_long_address(ip_dst);
	s=Get_long_address(ip_src);

	u_int16_t Icmp_type_code=0x0000;
	u_int8_t *payload = (u_int8_t*)malloc(320);
	memset(payload,0,320);


	l = libnet_init(
                LIBNET_LINK_ADV,                  /* injection type */
                device,                       /* network interface */
                errbuf);                      /* errbuf */

    if (l == NULL)
        {
            /* we should run through the queue and free any stragglers */
            zlog_debug(c, "libnet_init() failed: %s", errbuf);
            return 0;
        }
	while(Icmp_type_code <=0xffff)
	{

	
	u_int8_t Icmp_type=(u_int8_t)(Icmp_type_code>>8);
	u_int8_t Icmp_code=(u_int8_t)(Icmp_type_code);

	if (libnet_build_icmpv4_echo(Icmp_type, Icmp_code, 0, 0, 0,\
				        (u_int8_t*)payload,320, l, 0) == -1)
						{
								zlog_debug(c, "Error building ICMP echo header: %s\n",\
								      libnet_geterror(l));
								libnet_destroy(l);
								return 0;
						}

	if( libnet_build_ipv4(
						LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + sizeof(payload), \
						0,0x42,0,64,IPPROTO_ICMP,0,s,d,NULL,0,l,0)==-1)
					{
						zlog_debug(c, "Can't build IP header: %s\n", libnet_geterror(l));
						return 0;

					}

	if(libnet_build_ethernet(enet_dst,enet_src,0x0800,NULL,0,l,0)==-1)
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
			Icmp_type_code = Icmp_type_code+0x0001;
		}



	}

int frag_ping() {

	        int i;
	        char errbuf[LIBNET_ERRBUF_SIZE];
	        /* It's a good idea to have the payload as an array of bytes. If yours
	         * isn't, make a pointer to it and cast it.*/
	        u_int8_t payload[3000];
			libnet_t *l = NULL;
		

	        l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	        if ( l == NULL ) {
	                fprintf(stderr, "libnet_init() failed (raw4, 1st call): %s\n", \
	                                errbuf);
	                exit(EXIT_FAILURE);
	        }

	        /* Generating random payload */
	        libnet_seed_prand (l);

	        for (i = 0; i < sizeof(payload); i++) {
	                payload[i] = libnet_get_prand(LIBNET_PR8);
	        }

	        /* Building and sending the fragments */
	        frag_and_send(payload, sizeof(payload),l);

	        libnet_destroy(l);
	        return 0;
}

void frag_and_send(u_int8_t *payload, u_int32_t total_pload_size,libnet_t *l) {

        /* Builds and sends the first packet, calling get_sum() to get the 
         * correct checksum for the ICMP packet (with the whole payload). Then 
         * builds and sends IP fragments until all the payload is sent. */
		
        char ip_addr_str[16];
        u_int32_t ip_addr, src_addr;
        u_int16_t id, seq, ip_id;
        /* hdr_offset = fragmentation flags + offset (in bytes) divided by 8 */
        int pload_offset, hdr_offset; 
        int bytes_written, max_pload_size, packet_pload_size;
        libnet_ptag_t ip_tag;

        /* Generating random IDs */

        id = (u_int16_t)libnet_get_prand(LIBNET_PR16);
        /* We need a non-zero id number for the IP headers, otherwise libnet
         * will increase it after each build_ipv4, breaking the fragments */
        ip_id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

        seq = 1;

        /* Getting IP addresses */

        src_addr = libnet_get_ipaddr4(l);
        if ( src_addr == -1 ) {
                fprintf(stderr, "Couldn't get own IP address: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }

        printf("Destination IP address: ");
        scanf("%15s",ip_addr_str);

        ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);

        if ( ip_addr == -1 ) {
                fprintf(stderr, "Error converting IP address.\n");
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }

        /* Getting max payload size */

        max_pload_size = (MTU - LIBNET_IPV4_H);
        /* making it a multiple of 8 */
        max_pload_size -= (max_pload_size % 8);
        
        pload_offset = 0;

        /* Building the first packet, which carries the ICMP header */
        
        /* We're doing (payload size - icmp header size) and not
         * checking if it's a multiple of 8 because we know the header is 8 
         * bytes long */
        if ( total_pload_size > (max_pload_size - LIBNET_ICMPV4_ECHO_H) ) {
                hdr_offset = IP_MF;
                packet_pload_size = max_pload_size - LIBNET_ICMPV4_ECHO_H;
        }
        else {
                hdr_offset = 0;
                packet_pload_size = total_pload_size;
        }
        
        /* ICMP header */
        if ( libnet_build_icmpv4_echo(ICMP_ECHO, 0, get_sum(payload, \
                                        total_pload_size, id, seq), id, seq, \
                                payload, packet_pload_size, l, 0) == -1 )
        {
                fprintf(stderr, "Error building ICMP header: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }
        
        /* First IP header (no payload, offset == 0) */
        if ( libnet_build_ipv4((LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H \
                                        + packet_pload_size), 0, ip_id, \
                                hdr_offset, 255, IPPROTO_ICMP, 0, src_addr, \
                                ip_addr, NULL, 0, l, 0) == -1 )
        {
                fprintf(stderr, "Error building IP header: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }

        /* Writing packet */

        bytes_written = libnet_write(l);

        if ( bytes_written != -1 )
                printf("%d bytes written.\n", bytes_written);
        else
                fprintf(stderr, "Error writing packet: %s\n", \
                                        libnet_geterror(l));

        /* Updating the offset */
        pload_offset += packet_pload_size;

        /* Clearing */
        /* We need to get rid of the ICMP header to build the other
         * fragments */
        libnet_clear_packet(l);

        ip_tag = LIBNET_PTAG_INITIALIZER;

        /* Looping until all the payload is sent */
        while ( total_pload_size > pload_offset ) {
        
                /* Building IP header */

                /* checking if there will be more fragments */
                if ( (total_pload_size - pload_offset) > max_pload_size ) {
                        /* In IP's eyes, the ICMP header in the first packet
                         * needs to be in the offset, so we add its size to the
                         * payload offset here */
                        hdr_offset = IP_MF + (pload_offset + \
                                        LIBNET_ICMPV4_ECHO_H)/8;
                        packet_pload_size = max_pload_size;
                }
                else {
                        /* See above */
                        hdr_offset = (pload_offset + LIBNET_ICMPV4_ECHO_H)/8;
                        packet_pload_size = total_pload_size - pload_offset;
                }
        
                ip_tag = libnet_build_ipv4((LIBNET_IPV4_H + max_pload_size), \
                                0, ip_id, hdr_offset, 255, IPPROTO_ICMP, 0, \
                                src_addr, ip_addr, (payload + pload_offset), \
                                packet_pload_size, l, ip_tag);

                if ( ip_tag == -1 ) {
                        fprintf(stderr, "Error building IP header: %s\n", \
                                        libnet_geterror(l));
                        libnet_destroy(l);
                        exit(EXIT_FAILURE);
                }

                
                /* Writing packet */

                bytes_written = libnet_write(l);

                if ( bytes_written != -1 )
                        printf("%d bytes written.\n", bytes_written);
                else
                        fprintf(stderr, "Error writing packet: %s\n", \
                                        libnet_geterror(l));

                /* Updating the offset */
                pload_offset += packet_pload_size;
        }
}

u_int16_t get_sum(u_int8_t *payload, u_int32_t total_pload_size, \
                u_int16_t id, u_int16_t seq) {

        /* Builds the ICMP header with the whole payload, gets the checksum from
         * it and returns it (in host order). */

        char errbuf[LIBNET_ERRBUF_SIZE];
        libnet_ptag_t icmp_tag;
        u_int8_t *packet;
        u_int32_t packet_size;
        u_int16_t *sum_p, sum;
        u_int8_t dummy_dst[6] = {0, 0, 0, 0, 0, 0};
        
        icmp_tag = LIBNET_PTAG_INITIALIZER;
		libnet_t *l;

        /* Switching to advanced link mode */
        /* Nothing should be built yet and all random numbers should be already
         * generated. */
        libnet_destroy(l);
        l = libnet_init(LIBNET_LINK_ADV, NULL, errbuf);
        if ( l == NULL ) {
                fprintf(stderr, "libnet_init() failed (link_adv): %s\n", \
                                errbuf);
                exit(EXIT_FAILURE);
        }

        /* Building the header */
        icmp_tag = libnet_build_icmpv4_echo(ICMP_ECHO, 0, 0, id, seq, \
                        payload, total_pload_size, l, icmp_tag);
        
        if ( icmp_tag == -1 ) {

                fprintf(stderr, "Error building ICMP header: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }

        /* Building dummy IP header */
        if ( libnet_autobuild_ipv4((LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + \
                                        total_pload_size), \
                                IPPROTO_ICMP, 0, l) == -1 ) {
                fprintf(stderr,        "Error building dummy IP header: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }

        /* Building dummy Ethernet header */
        if ( libnet_autobuild_ethernet (dummy_dst, ETHERTYPE_IP, l) == -1 ) {
                fprintf(stderr,        "Error building dummy Ethernet header: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }


        /* Pulling the packet */
        if (libnet_adv_cull_packet(l, &packet, &packet_size) == -1) {
                fprintf(stderr, "Error pulling the packet: %s\n", \
                                libnet_geterror(l));
                libnet_destroy(l);
                exit(EXIT_FAILURE);
        }

        /* Grabbing the checksum */
        /* We want the 37th and 38th bytes: eth header (14) + ip header (20) +
         * icmp type and code (2) = 36 */
        sum_p = (u_int16_t*)(packet + 36); 
        sum = ntohs(*sum_p);

        /* Freeing memory */
        libnet_adv_free_packet(l, packet);

        /* Clearing the header */
        libnet_clear_packet(l);

        /* Switching back to IPv4 raw socket mode */
        libnet_destroy(l);
        l = libnet_init(LIBNET_RAW4, NULL, errbuf);
        if ( l == NULL ) {
                fprintf(stderr, "libnet_init() failed (raw4, 2nd call): %s\n", errbuf);
                exit(EXIT_FAILURE);
        }

        return sum;
}


int ping() {



	

  libnet_t *l;  /* libnet context */
  char errbuf[LIBNET_ERRBUF_SIZE], ip_addr_str[16];
  u_int32_t ip_addr;
  u_int16_t id, seq;
  char payload[] = "libnet :D";
  int bytes_written;
  u_char enet_src[6] = {0x0d, 0x0e, 0x0a, 0x0d, 0x00, 0x00};
	u_char enet_dst[6] = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};

  	l = libnet_init(
                LIBNET_LINK_ADV,                  /* injection type */
                "eth0",                       /* network interface */
                errbuf);                      /* errbuf */
  if ( l == NULL ) {
    fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  /* Generating a random id */

  libnet_seed_prand (l);
  id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

  /* Getting destination IP address */

  printf("Destination IP address: ");
  scanf("%15s",ip_addr_str);

  ip_addr = libnet_name2addr4(l, ip_addr_str,\
                  LIBNET_DONT_RESOLVE);

  if ( ip_addr == -1 ) {
    fprintf(stderr, "Error converting IP address.\n");
    libnet_destroy(l);
    exit(EXIT_FAILURE);
  }

  /* Building ICMP header */

  seq = 1;

  if (libnet_build_icmpv4_echo(0x00, 0x00, 0, id, seq,\
        (u_int8_t*)payload,sizeof(payload), l, 0) == -1)
  {
    fprintf(stderr, "Error building ICMP header: %s\n",\
        libnet_geterror(l));
    libnet_destroy(l);
    exit(EXIT_FAILURE);
  }

  /* Building IP header */
  u_int32_t d=Get_long_address("192.168.1.2");
  u_int32_t s=Get_long_address("192.168.1.5");

  if (libnet_autobuild_ipv4(LIBNET_IPV4_H +\
        LIBNET_ICMPV4_ECHO_H + sizeof(payload),\
        IPPROTO_ICMP, ip_addr, l) == -1 )
  {
    fprintf(stderr, "Error building IP header: %s\n",\
        libnet_geterror(l));
    libnet_destroy(l);
    exit(EXIT_FAILURE);
  }


 libnet_build_ethernet(
							 enet_dst,						  /* 目标主机的MAC地址 */
							 enet_src,						  /* 发送主机的MAC地址 */
							 0x0800,					   
							 NULL,
							 0,
							 l,
							 0);

  /* Writing packet */
while(1)
{
  bytes_written = libnet_write(l);
  if ( bytes_written != -1 )
    printf("%d bytes written.\n", bytes_written);
  else
    fprintf(stderr, "Error writing packet: %s\n",\
        libnet_geterror(l));
}

  libnet_destroy(l);
  return 0;
}



