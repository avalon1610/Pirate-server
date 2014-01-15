
#include "network.h"
#include "arp.h"
#include <time.h>
#include "comm.h"
#include "zlog.h"



extern zlog_category_t *c;

int ARP_Request_Storm(ARP_REQUEST_STORM *a)
{
	zlog_debug(c,"Start ARP_Request_Storm\n");
	RUNING_MISSION_W(pthread_self(),RUNNING,clock(),ARP_REQUEST_STORM_);

	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->enet_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	int storm_size=a->storm_size;
	int test_time=a->test_time;
	int space_time=a->space_time;
	zlog_debug(c,"mission arg:device:%s,storm_size:%d,test_time:%d,space_time:%d\n");
	
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
	   DbgPrint( "%s", errbuf);
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
        DbgPrint("Can't build ARP header: %s\n", libnet_geterror(l));
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
        DbgPrint("%s", libnet_geterror(l));
    }
    else
    {
        DbgPrint("packet size: %d\n", packet_s);
        libnet_adv_free_packet(l, packet);
    }

	if(space_time)
		{
			send_storm_set_time(l,storm_size,packet_s,test_time,space_time);
		}
	else
		{
			send_storm_random_time(l,storm_size,packet_s,test_time);
		}

    libnet_destroy(l);
	RUNING_MISSION_W(0,END,clock(),ARP_REQUEST_STORM_);
    return 1;



}


int APR_Host_Reply_Storm(APR_HOST_REPLY_STORM *a)
{
	u_char *ip_dst=a->ip_dst;
	u_char *ip_src=a->ip_src;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	int storm_size=a->storm_size;
	int test_time=a->test_time;
	int space_time=a->space_time;
	
	int last=1;
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
	
	if(space_time)
		{
			send_storm_set_time(l,storm_size,packet_s,STORM_TIME,space_time);
		}
	else
		{
			send_storm_random_time(l,storm_size,packet_s,STORM_TIME);
		}



    libnet_destroy(l);
    return 1;



}

int ARP_Grammear(ARP_GRAMMEAR *a)
{		
	u_char *ip_dst=a->ip_dst;
	u_char *enet_src=a->enet_src;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;

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




/*ARP Cache Saturation Storm APR高速缓存测试
主要测试方式就是在发风暴数据报文的屯�
不断的改变本身的IP地址和MAC地址*/
int ARP_Cache_Saturation_Storm(ARP_CACHE_SATURATION_STORM *a)
{

	u_char *ip_dst=a->ip_dst;
	u_char *enet_dst=a->enet_dst;
	char *device=a->device;
	int storm_size=a->storm_size;
	int space_time=a->space_time;
	int c;
	
   
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
	
	
	int send_size=0;
	clock_t test;
	clock_t start=test= clock();
    clock_t end = (clock() - start)/CLOCKS_PER_SEC;

	int R=0;
	srand((unsigned)time(NULL));
	
while((end-start)/CLOCKS_PER_SEC<=STORM_TIME){
	if(space_time && (end-test)/CLOCKS_PER_SEC == space_time )
		{
			ARP_Cache_Send(l,enet_dst,storm_size,d);
			test=clock();
		}
	else
		{
			
			if((end-test)/CLOCKS_PER_SEC == R)
				{	
				
					ARP_Cache_Send(l,enet_dst,storm_size,d)	;
					test=clock();
					R=rand()%10;
					
				}
		}
		end =clock();
}
	
	libnet_destroy(l);		 
	return 1;
}

int ARP_Cache_Send(libnet_t *l,u_char *enet_dst,int storm_size,u_int32_t d)
{
	u_char enet_src[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	int send_size=0;
	int c;
	libnet_ptag_t t;
    u_int8_t *packet;
    u_int32_t packet_s;
while(send_size<= storm_size)
{
	int rand_s_ip=rand();
	int rand2=rand();
	int rand3=rand();
   	memcpy(enet_src,&rand2,sizeof(int));
   	memcpy(enet_src+4,&rand3,2);
	
	 t = libnet_build_arp(
			 ARPHRD_ETHER,							 /* hardware addr */
			 ETHERTYPE_IP,							 /* protocol addr */
			 6, 									 /* hardware addr size */
			 4, 									 /* protocol addr size */
			 ARPOP_REPLY,							 /* operation type */
			 enet_src,								 /* sender hardware addr */
			 (u_int8_t *)&rand_s_ip,				   /* sender protocol addr */
			 enet_dst,								 /* target hardware addr */
			 (u_int8_t *)&d,						 /* target protocol addr */
			 NULL,									 /* payload */
			 0, 									 /* payload size */
			 l, 									 /* libnet context */
			 0);									 /* libnet id */
	 if (t == -1)
	 {
		 fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(l));
	 }

   	t = libnet_build_ethernet(
				   enet_dst,						/* 目标主机的MAC地址 */
				   enet_src,						/* 发送主机的MAC地址 */
				   ETHERTYPE_ARP,						 /* 以太网帧类型，这里是ARP */
				   NULL,
				   0,
				   l,
				   0);
	// t = libnet_autobuild_ethernet(
   //		 enet_dst,								 /* ethernet destination */
	   //	 ETHERTYPE_ARP, 						 /* protocol type */
	   //	 l);									 /* libnet handle */
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
			//		 "check the wire.\n", c, libnet_cq_getlabel(l));
	   }
	 
	 libnet_clear_packet(l);

}

}

