#ifndef __DEFINES_H__
#define __DEFINES_H__
 


 /* should packet counters be 32 or 64 bit? --enable-64bit */
#ifdef ENABLE_64BITS
#define COUNTER unsigned long long
#define COUNTER_SPEC "%llu"
#else
#define COUNTER unsigned long
#define COUNTER_SPEC "%lu"
#endif

#ifdef HAVE_BPF
#include <net/bpf.h>
#define PCAP_DONT_INCLUDE_PCAP_BPF_H 1
#endif
#if defined INCLUDE_PCAP_BPF_H_FILE && !defined PCAP_DONT_INCLUDE_PCAP_BPF_H
#include "/usr/include/pcap-bpf.h"
#define PCAP_DONT_INCLUDE_PCAP_BPF_H 1 /* don't re-include it in pcap.h */
#endif

#include "/usr/include/pcap.h"

 
/* 
* net/bpf.h doesn't include DLT types, but pcap-bpf.h does.  
* Unfortunately, pcap-bpf.h also includes things in net/bpf.h
* while also missing some key things (wow, that sucks)
 * The result is that I stole the DLT types from pcap-bpf.h and 
* put them in here.
*/

#ifdef HAVE_LIBNET
#include "@LNETINC@"
#endif

typedef struct tcpr_ipv4_hdr ipv4_hdr_t;
typedef struct tcpr_ipv6_hdr ipv6_hdr_t;
typedef struct tcpr_tcp_hdr tcp_hdr_t;
typedef struct tcpr_udp_hdr udp_hdr_t;
typedef struct tcpr_icmpv4_hdr icmpv4_hdr_t;
typedef struct tcpr_icmpv6_hdr icmpv6_hdr_t;
typedef struct tcpr_ethernet_hdr eth_hdr_t;
typedef struct tcpr_802_1q_hdr vlan_hdr_t;
typedef struct sll_header sll_hdr_t;
typedef struct tcpr_arp_hdr arp_hdr_t;
typedef struct tcpr_dnsv4_hdr dnsv4_hdr_t;
 
/* our custom typdefs/structs */

struct tcpr_bpf_s {
    char *filter;
    int optimize;
    struct bpf_program program;
};
typedef struct tcpr_bpf_s tcpr_bpf_t;

 typedef struct tcpr_xX_s tcpr_xX_t;
 
 /* number of ports 0-65535 */
 #define NUM_PORTS 65536
 struct tcpr_services_s {
    char tcp[NUM_PORTS];
    char udp[NUM_PORTS];
 };
 typedef struct tcpr_services_s tcpr_services_t;
 
 struct tcpr_speed_s {
     /* speed modifiers */
     int mode;
 #define SPEED_MULTIPLIER 1
 #define SPEED_MBPSRATE   2
 #define SPEED_PACKETRATE 3
 #define SPEED_TOPSPEED   4
 #define SPEED_ONEATATIME 5
     float speed;
     int pps_multi;
 };
 typedef struct tcpr_speed_s tcpr_speed_t;
 
 #define MAX_FILES   1024        /* Max number of files we can pass to tcpreplay */
 
 #define DEFAULT_MTU 1500        /* Max Transmission Unit of standard ethernet
                                  * don't forget *frames* are MTU + L2 header! */

 #define MAXPACKET 65535         /* was 16436 linux loopback, but maybe something is bigger then 
                                   linux loopback */
 
 #define MAX_SNAPLEN 65535       /* tell libpcap to capture the entire packet */
 
 #define DNS_RESOLVE 1
 #define DNS_DONT_RESOLVE 0
 
 #define RESOLVE 0               /* disable dns lookups */
 #define BPF_OPTIMIZE 1          /* default is to optimize bpf program */
 #define PCAP_TIMEOUT 100        /* 100ms pcap_open_live timeout */
 
 /* HP-UX already defines TRUE/FALSE */
 #ifndef TRUE
 enum bool_t {
     FALSE = 0,
     TRUE
 };
 #endif

 #define EBUF_SIZE 1024           /* size of our error buffers */
 #define MAC_SIZE  7             /* size of the mac[] buffer */
 
 enum pad_t {
     PAD_PACKET,
     TRUNC_PACKET
 };
 
 #define DNS_QUERY_FLAG 0x8000
 
 enum direction_t {
     DIR_UNKNOWN = -1,
     DIR_CLIENT = 0,
    DIR_SERVER = 1,
     DIR_ANY = 2
 };
 

#define BROADCAST_MAC "\xFF\xFF\xFF\xFF\xFF\xFF"
 
/* MAC macros for printf */
#define MAC_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"
#define MAC_STR(x) x[0], x[1], x[2], x[3], x[4], x[5]

 /* struct timeval print structs */
#ifdef HAVE_DARWIN
/* Darwin defines usec as an __int32_t, not unsigned long. */
#define TIMEVAL_FORMAT "%lus %dusec"
#else
#define TIMEVAL_FORMAT "%lus %luusec"
#endif
#define TIMESPEC_FORMAT "%lus %lunsec"

/* force a word or half-word swap on both Big and Little endian systems */
 #ifndef SWAPLONG
 #define SWAPLONG(y) \
 ((((y)&0xff)<<24) | (((y)&0xff00)<<8) | (((y)&0xff0000)>>8) | (((y)>>24)&0xff))
 #endif
 
 #ifndef SWAPSHORT
 #define SWAPSHORT(y) \
( (((y)&0xff)<<8) | ((u_short)((y)&0xff00)>>8) )
 #endif
 
/* converts a 64bit int to network byte order */
 #ifndef HAVE_NTOHLL
 #ifdef WORDS_BIGENDIAN
 #define ntohll(x) (x)
 #define htonll(x) (x)
 #else
 /* stolen from http://www.codeproject.com/cpp/endianness.asp */
 #define ntohll(x) (((u_int64_t)(ntohl((int)((x << 32) >> 32))) << 32) | \
                      (unsigned int)ntohl(((int)(x >> 32))))
 #define htonll(x) ntohll(x)
 #endif /* WORDS_BIGENDIAN */
 #endif
 



/* convert IPv6 Extention Header Len value to bytes */
 #define IPV6_EXTLEN_TO_BYTES(x) ((x * 4) + 8)
 

 
 /* Support for flexible arrays. */
 #undef __flexarr
 #if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 97))
 /* GCC 2.97 supports C99 flexible array members.  */
 # define __flexarr      []
 #else
 # ifdef __GNUC__
 #  define __flexarr     [0]
 # else
 #  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
 #   define __flexarr    []
 #  elif defined(_WIN32)
 /* MS VC++ */
 #   define __flexarr    []
 #  else
 /* Some other non-C99 compiler. Approximate with [1]. */
 #   define __flexarr    [1]
 #  endif
 # endif
 #endif

 
 /* Time converters */
 #define SEC_TO_MILLISEC(x) (x * 1000)
 #define SEC_TO_MICROSEC(x) (x * 1000000)
 #define SEC_TO_NANOSEC(x)  ((u_int64_t)x * 1000000000)
 
 #define MILLISEC_TO_SEC(x) (x / 1000)
 #define MICROSEC_TO_SEC(x) (x / 1000000)
 #define NANOSEC_TO_SEC(x)  ((u_int64_t)x / 1000000000)
 
 #define TIMEVAL_TO_MILLISEC(x)  (((x)->tv_sec * 1000) + ((x)->tv_usec / 1000))
 #define TIMEVAL_TO_MICROSEC(x)  (((x)->tv_sec * 1000000) + (x)->tv_usec)
 #define TIMEVAL_TO_NANOSEC(x)   ((u_int64_t)((x)->tv_sec * 1000000000) + ((u_int64_t)(x)->tv_usec * 1000))
 
 #define MILLISEC_TO_TIMEVAL(x, tv)          \
     do {                                    \
         (tv)->tv_sec = (x) / 1000;          \
         (tv)->tv_usec = (x * 1000) - ((tv)->tv_sec * 1000000);   \
     } while(0)
 
 #define MICROSEC_TO_TIMEVAL(x, tv)          \
     do {                                    \
         (tv)->tv_sec = (x) / 1000000;       \
         (tv)->tv_usec = (x) - ((tv)->tv_sec * 1000000);   \
     } while(0)
 
 #define NANOSEC_TO_TIMEVAL(x, tv)           \
     do {                                    \
         (tv)->tv_sec =  (x) / 1000000000;   \
         (tv)->tv_usec = ((x) % 1000000000) / 1000);   \
     } while(0)
 
 #define NANOSEC_TO_TIMESPEC(x, ts)          \
     do {                                    \
         (ts)->tv_sec = (x) / 1000000000;    \
         (ts)->tv_nsec = (x) % 1000000000;   \
     } while(0)
 
 #define TIMESPEC_TO_MILLISEC(x)  (((x)->tv_sec * 1000) + ((x)->tv_nsec / 1000000))
 #define TIMESPEC_TO_MICROSEC(x)  (((x)->tv_sec * 1000000) + (x)->tv_nsec / 1000)
 #define TIMESPEC_TO_NANOSEC(x) ((u_int64_t)((x)->tv_sec * 1000000000) + ((u_int64_t)(x)->tv_nsec))
 
#define do_div(n,base) ({        \
	 uint32_t __base = (base);	   \
	 uint32_t __rem;		 \
	 if (((n) >> 32) == 0) {	 \
		 __rem = (uint32_t)(n) % __base;   \
		 (n) = (uint32_t)(n) / __base;	 \
	 }			  \
	})


 #endif /* DEFINES */

