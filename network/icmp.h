#ifndef _ICMP_H_
#define _ICMP_H_

#include "define.h"
#include "mylist.h"

typedef struct _ICMP_STORM_STORM
{

		u_char ip_dst[16];
		u_char ip_src[16];
		u_char enet_src[6];
		u_char enet_dst[6];
		char device[10];
		COUNTER speed;
		int test_time;
		int storm_time;
		bool top_speed;

}ICMP_STORM_STORM,*PICMP_STORM_STORM;


typedef struct _ICMP_FUZZER
{

		u_char ip_dst[16];
		u_char ip_src[16];
		u_char enet_src[6];
		u_char enet_dst[6];
		char device[10];
		COUNTER speed;
		bool top_speed;

}ICMP_FUZZER,*PICMP_FUZZER;



typedef struct _ICMP_GRAMMAR
{

		u_char ip_dst[16];
		u_char ip_src[16];
		u_char enet_src[6];
		u_char enet_dst[6];
		char device[10];
		COUNTER speed;
		bool top_speed;

}ICMP_GRAMMAR,*PICMP_GRAMMAR;


typedef struct _ICMP_TYPE_CODE
{

		u_char ip_dst[16];
		u_char ip_src[16];
		u_char enet_src[6];
		u_char enet_dst[6];
		char device[10];
		COUNTER speed;
		bool top_speed;

}ICMP_TYPE_CODE,*PICMP_TYPE_CODE;



int Icmp_storm(ICMP_STORM_STORM *a );
int Icmp_Fuzzer(ICMP_FUZZER *a );
int Icmp_Grammar(ICMP_GRAMMAR *a);
int Icmp_Type_Code_Cross_Product(ICMP_TYPE_CODE *a);





#endif


