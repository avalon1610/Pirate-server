#ifndef _NETWORK_H_
#define _NETWORK_H_
#include  "/usr/include/libnet.h"
#include "comm.h"
#include "timer.h"
#include "ethernet.h"
#include "icmp.h"


#define STORM_TIME 600

#define do_div(n,base) ({          \
    uint32_t __base = (base);       \
    uint32_t __rem;           \
    __rem = ((uint32_t)(n)) % __base;     \
    (n) = ((uint32_t)(n)) / __base;       \
    __rem;              \
   })

# define do_div_64(n,base) ({          \
    uint32_t __base = (base);       \
    uint32_t __rem;           \
    __rem = ((uint64_t)(n)) % __base;     \
    (n) = ((uint64_t)(n)) / __base;       \
    __rem;              \
   })



uint32_t __div64_32(uint64_t *n, uint32_t base);

int send_storm(libnet_t *lib_net,COUNTER speed,clock_t storm_time,bool top_speed);
u_int32_t Get_long_address(u_char *ip_address);
void Create_Random(int len,u_char *Rd);
int icmp_storm(u_char *ip_dst,u_char *ip_src,u_char *enet_src,u_char *enet_dst,char *device,int storm_size);
int send_storm_set_time(libnet_t *lib_net,COUNTER speed,clock_t test_time,clock_t storm_time,bool top_speed);
int send_storm_random_time(libnet_t *lib_net,COUNTER speed,int test_time,bool top_speed);





#ifdef __cplusplus
extern "C" {
#endif
void Test_Work(COMMAND *a);
void do_sleep(ACCURATE accurate ,delta_t *delta_ctx,struct timeval *start_time,COUNTER speed,COUNTER send_size,int len ,bool *skip_timestamp);

#ifdef __cplusplus
}
#endif

#endif


