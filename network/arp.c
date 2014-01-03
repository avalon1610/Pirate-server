
#include "network.h"
#include "arp.h"
#include <time.h>




int ARP_Request_Storm(struct ARP_Request_Storm_ARG *a)
{

	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->enet_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	int storm_size=a->storm_size;
	u_int32_t d;
	u_int32_t s;
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
	s = Get_long_address(ip_src);

	d = Get_long_address(ip_dst);
	t = libnet_build_arp(
	            ARPHRD_ETHER,                           /* hardware addr */
	            ETHERTYPE_IP,                           /* protocol addr */
	            6,                                      /* hardware addr size */
	            4,                                      /* protocol addr size */
	            ARPOP_REPLY,                            /* operation type */
	            enet_src,                               /* sender hardware addr */
	            (u_int8_t *)&s,                         /* sender protocol addr */
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
	clock_t start = clock();
    clock_t end = (clock() - start)/CLOCKS_PER_SEC;
	clock_t last;
	while(end<10)
		{	
			
			send_storm(l,storm_size,packet_s);
			end = (clock() - start)/CLOCKS_PER_SEC;
		}

    libnet_destroy(l);
    return 1;



}


int APR_Host_Reply_Storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size)
{
	u_int32_t i;
	u_int32_t d;
	u_int32_t s;
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
	s = Get_long_address(ip_src);;	
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

int ARP_Grammear(u_char *ip_dst,u_char *enet_src,u_char *enet_dst,char *device)
{		
		int c;
		u_int32_t i;
		u_int32_t d;
		libnet_t *l;
		libnet_ptag_t t;
		u_int8_t *packet;
		u_int32_t packet_s;
		u_int16_t ARP_POROTOL=0x0800;
		u_int16_t ARPHRD_TYPE=0x0001;
		u_int16_t OPCODE=0x0001;
		char errbuf[LIBNET_ERRBUF_SIZE];
		u_char enet_000[6]={0x00,0x00,0x00,0x00,0x00,0x00};
		//u_char enet_fff[6]={0xff,0xff,0xff,0xff,0xff,0xff};
		/*Create a arry to store the Grammear ARPHRD_TYPE,ARP_POROTOL ,OPCODE*/
		u_int16_t Grammear_ARPHRD_TYPE[21]={
		0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000a,
		0x000b,0x000c,0x000d,0x000e,0x000f,0x0100,0x2573,0x7325,0xfffe,0xffff
		};
		u_int16_t Grammear_ARP_POROTOL[20]={
		0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000a,
		0x00fe,0x00ff,0x0100,0x0806,0x8035,0x86dd,0x7325,0xfffe,0xffff
		};
		u_int16_t Grammear_ARP_OPCODE[20]={
		0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000a,
		0x00fe,0x00ff,0x0100,0x0806,0x8035,0x86dd,0x7325,0xfffe,0xffff
		};
		u_int8_t Grammear_ARP_HARDWARE_SIZE[12]={
		0x00,0x01,0x02,0x03,0x04,0x05,0x07,0x25,0x57,0x60,0xfe,0xff
		};
		u_int8_t Grammear_ARP_POROTOL_SIZE[12]={
		0x00,0x01,0x03,0x05,0x07,0x25,0x57,0x60,0xfe,0xff
		};
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
		/*1. Test change ARPHRD_TYPE by Grammear_ARPHRD_TYPE[21]*/
		int j=0;
		int m=0;
		for(j=0;j<21;j++)
		{	
			
				t = libnet_build_arp(
						Grammear_ARPHRD_TYPE[j],							/* hardware addr */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			c=libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
		}
		/*2. Test change POROTOL by Grammear_ARP_POROTOL[21]*/
		for(j=0;j<20;j++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						Grammear_ARP_POROTOL[j],							/* protocol addr */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
		}

		/*3. Test change POROTOL AND  ARPHRD_TYPE*/
	for(j=0;j<20;j++){
		for(m=0;m<20;m++)
		{	
			
				t = libnet_build_arp(
						Grammear_ARPHRD_TYPE[m],							/* hardware addr */
						Grammear_ARP_POROTOL[j],							/* protocol addr */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		}
		/*4. Test change ARP_HARDWARE_SIZE AND  ARP_POROTOL_SIZE*/
	for(j=0;j<12;j++){
		for(m=0;m<10;m++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						ARP_POROTOL,							/* protocol addr */
						Grammear_ARP_HARDWARE_SIZE[j],										/* hardware addr size */
						Grammear_ARP_POROTOL_SIZE[m],										/* protocol addr size */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		}
	/*5. Test change ARP_HARDWARE_SIZE AND  ARPHRD_TYPE*/
	for(j=0;j<12;j++){
		for(m=0;m<21;m++)
		{	
			
				t = libnet_build_arp(
						Grammear_ARPHRD_TYPE[m],							/* hardware addr */
						ARP_POROTOL,							/* protocol addr */
						Grammear_ARP_HARDWARE_SIZE[j],										/* hardware addr size */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		
		for(m=0;m<20;m++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						Grammear_ARP_POROTOL[20],							/* protocol addr */
						Grammear_ARP_HARDWARE_SIZE[j],										/* hardware addr size */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		}
	/*6. Test change POROTOL_SIZE AND	ARPHRD_TYPE*/


	for(j=0;j<12;j++){
		for(m=0;m<21;m++)
		{	
			
				t = libnet_build_arp(
						Grammear_ARPHRD_TYPE[m],							/* hardware addr */
						ARP_POROTOL,							/* protocol addr */
						6,										/* hardware addr size */
						Grammear_ARP_POROTOL_SIZE[j],										/* protocol addr size */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		
		for(m=0;m<20;m++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						Grammear_ARP_POROTOL[20],							/* protocol addr */
						6,										/* hardware addr size */
						Grammear_ARP_POROTOL_SIZE[j],										/* protocol addr size */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		}
		/*7. Test change OPCODE */
	for(j=0;j<20;j++){
		for(m=0;m<21;m++)
		{	
			
				t = libnet_build_arp(
						Grammear_ARPHRD_TYPE[m],							/* hardware addr */
						ARP_POROTOL,							/* protocol addr */
						6,										/* hardware addr size */
						4,										/* protocol addr size */
						Grammear_ARP_OPCODE[j],							        /* operation type */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		
		for(m=0;m<20;m++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						Grammear_ARP_POROTOL[20],							/* protocol addr */
						6,										/* hardware addr size */
						4,										/* protocol addr size */
						Grammear_ARP_OPCODE[j],							        /* operation type */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}

		for(m=0;m<12;m++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						ARP_POROTOL,							/* protocol addr */
						Grammear_ARP_HARDWARE_SIZE[m],										/* hardware addr size */
						4,										/* protocol addr size */
						Grammear_ARP_OPCODE[j],							        /* operation type */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}


		for(m=0;m<12;m++)
		{	
			
				t = libnet_build_arp(
						ARPHRD_TYPE,							/* hardware addr */
						ARP_POROTOL,							/* protocol addr */
						6,										/* hardware addr size */
						Grammear_ARP_POROTOL_SIZE[m],										/* protocol addr size */
						Grammear_ARP_OPCODE[j],							        /* operation type */
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
					l); 		
			if (t == -1)
			{
				fprintf(stderr, "Can't build ethernet header: %s\n",
						libnet_geterror(l));
				return 0;
			}/* libnet handle */
			if (libnet_adv_cull_packet(l, &packet, &packet_s) == -1)
			{
				//fprintf(stderr, "%s", libnet_geterror(l));
			}
			else
			{
				//fprintf(stderr, "packet size: %d\n", packet_s);
				libnet_adv_free_packet(l, packet);
			}
			c = libnet_write(l);
			if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
			libnet_clear_packet(l);
			}
		}
	
		libnet_destroy(l);
		return 1;
	


}




/*ARP Cache Saturation Storm APR¸ßËÙ»º´æ²âÊÔ
Ö÷Òª²âÊÔ·½Ê½¾ÍÊÇÔÚ·¢·ç±©Êý¾Ý±¨ÎÄµÄÍÍ¬Ê±
²»¶ÏµÄ¸Ä±ä±¾ÉíµÄIPµØÖ·ºÍMACµØÖ·*/
int ARP_Cache_Saturation_Storm(u_char *ip_dst,u_char *enet_dst,char *device,int storm_size)
{

	int c;
	int send_size=0;
   
    libnet_t *l;
	u_int32_t d;
    libnet_ptag_t t;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	
	u_char enet_src[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	d = Get_long_address(ip_dst);
	l = libnet_init(
			  LIBNET_LINK_ADV,						  /* injection type */
			  device,								  /* network interface */
			  errbuf);								  /* errbuf */

	  if (l == NULL)
	  {
		  fprintf(stderr, "%s", errbuf);
		  return 0;;
	  }

	while(send_size<storm_size)
	{
     /***RAND to create a random ****/
		 int rand_s_ip=rand();
		 int rand2=rand();
		 int rand3=rand();
		 
		memcpy(enet_src,&rand2,sizeof(int));
		memcpy(enet_src+4,&rand3,2);
		 
		  t = libnet_build_arp(
				  ARPHRD_ETHER, 						  /* hardware addr */
				  ETHERTYPE_IP, 						  /* protocol addr */
				  6,									  /* hardware addr size */
				  4,									  /* protocol addr size */
				  ARPOP_REPLY,							  /* operation type */
				  enet_src, 							  /* sender hardware addr */
				  (u_int8_t *)&rand_s_ip,					/* sender protocol addr */
				  enet_dst, 							  /* target hardware addr */
				  (u_int8_t *)&d,						  /* target protocol addr */
				  NULL, 								  /* payload */
				  0,									  /* payload size */
				  l,									  /* libnet context */
				  0);									  /* libnet id */
		  if (t == -1)
		  {
			  fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(l));
		  }

		t = libnet_build_ethernet(
        				enet_dst,                        /* Ä¿±êÖ÷»úµÄMACµØÖ· */
        				enet_src,                        /* ·¢ËÍÖ÷»úµÄMACµØÖ· */
        				ETHERTYPE_ARP,                        /* ÒÔÌ«ÍøÖ¡ÀàÐÍ£¬ÕâÀïÊÇARP */
        				NULL,
        				0,
        				l,
        				0);
		 // t = libnet_autobuild_ethernet(
		//		  enet_dst, 							  /* ethernet destination */
			//	  ETHERTYPE_ARP,						  /* protocol type */
			//	  l);									  /* libnet handle */
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
		  
		  	send_size=send_size+packet_s;
			libnet_adv_free_packet(l, packet);
		  }
		  c = libnet_write(l);
		  	if (c == -1)
		    {
		        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
		    }
		    else
		    {
		        //fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
		         //       "check the wire.\n", c, libnet_cq_getlabel(l));
		    }
		  
		  libnet_clear_packet(l);
		  
	}
	return 1;
}

int 
main_test(int argc, char *argv[])
{	
	
	u_char enet_src[6] = {0x0d, 0x0e, 0x0a, 0x0d, 0x00, 0x00};
	u_char enet_dst[6] = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};
	u_char ip_src[4]   = {0x0a, 0x00, 0x00, 0x01};
	u_char ip_dst[4]   = {0x0a, 0x00, 0x00, 0x02};
	u_char fddi_src[6] = {0x00, 0x0d, 0x0e, 0x0a, 0x0d, 0x00};
	u_char fddi_dst[6] = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};
	u_char tr_src[6]   = {0x00, 0x0d, 0x0e, 0x0a, 0x0d, 0x00};
	u_char tr_dst[6]   = {0x00, 0x10, 0x67, 0x00, 0xb1, 0x86};

	//int re=ARP_Grammear("192.168.1.1","192.168.1.100",enet_src,enet_dst,"eth0");
	//int re= ARP_Cache_Saturation_Storm("192.168.1.100",enet_dst,"eth0",1000000);
	//int re=icmp_storm("192.168.1.1","192.168.1.100",enet_src,enet_dst,"eth0",100000);
    //int c;
    u_int32_t i;
    libnet_t *l;
    libnet_ptag_t t;
    char *device = NULL;
    u_int8_t *packet;
    u_int32_t packet_s;
    char errbuf[LIBNET_ERRBUF_SIZE];
	//int pcap_count=0;

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
	
	//send_storm(l,10000,packet_s);

    libnet_destroy(l);
    return (EXIT_SUCCESS);
bad:
    libnet_destroy(l);
    return (EXIT_FAILURE);
}




