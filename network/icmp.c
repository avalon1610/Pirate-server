#include "./include/network.h"

/*
 *  $Id: icmp_echo_cq.c,v 1.3 2004/01/03 20:31:01 mike Exp $
 *
 *  libnet 1.1
 *  Build ICMP_ECHO packets using the context queue interface.
 *
 *  Copyright (c) 1998 - 2004 Mike D. Schiffman <mike@infonexus.com>
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


void usage(char *);


int icmp_storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size)
{
    libnet_t *l = NULL;
    u_long src_ip, dst_ip ;
    u_long count = 10;
    int i, c;
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
    /*
     *  Fill the context queue with "count" packets, each with their own
     *  context.
     */

        l = libnet_init(
                LIBNET_RAW4,                  /* injection type */
                device,                       /* network interface */
                errbuf);                      /* errbuf */

        if (l == NULL)
        {
            /* we should run through the queue and free any stragglers */
            fprintf(stderr, "libnet_init() failed: %s", errbuf);
            exit(EXIT_FAILURE);
        }
        /*
         *  Since we need a libnet context for address resolution it is
         *  necessary to put this inside the loop.
         */
        if (!dst_ip && (dst_ip = libnet_name2addr4(l, ip_dst,
                LIBNET_RESOLVE)) == -1)
        {
            fprintf(stderr, "Bad destination IP address: %s\n", pDst);
            return 0;
        }
        if (!src_ip && (src_ip = libnet_name2addr4(l, ip_src,
                LIBNET_RESOLVE)) == -1)
        {
            fprintf(stderr, "Bad source IP address: %s\n", pSrc);
            return 0;
        }

	
	        t = libnet_build_icmpv4_echo(
	            icmp_type,                            /* type */
	            0,                                    /* code */
	            0,                                    /* checksum */
	            0x42,                                 /* id */
	            0x42,                                 /* sequence number */
	            icmp_payload,                                 /* payload */
	            20,                                    /* payload size */
	            l,                                    /* libnet handle */
	            0);
	        if (t == -1)
	        {
	            fprintf(stderr, "Can't build ICMP header: %s\n",
	                    libnet_geterror(l));
	            return 0;
	        }

	        t = libnet_build_ipv4(
	            LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_s, /* length */
	            0,                                    /* TOS */
	            0x42,                                 /* IP ID */
	            0,                                    /* IP Frag */
	            64,                                   /* TTL */
	            IPPROTO_ICMP,                         /* protocol */
	            0,                                    /* checksum */
	            src_ip,                               /* source IP */
	            dst_ip,                               /* destination IP */
	            payload,                              /* payload */
	            payload_s,                            /* payload size */
	            l,                                    /* libnet handle */
	            0);
	        if (t == -1)
	        {
	            fprintf(stderr, "Can't build IP header: %s\n", libnet_geterror(l));
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
			
	        if (c == -1)
	        {
	            fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
	            return 0;
	        }

		
			
			
	
  
    return 1;

}

void
usage(char *name)
{
    fprintf(stderr, "usage: %s -s source_ip -d destination_ip"
                    " [-i iface] [-c count = 10]\n ", name);
}


