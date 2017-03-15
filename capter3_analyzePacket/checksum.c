#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<sys/socket.h>
#include<linux.if.h>
#include<net/ethernet.h>
#include<arpa/inet.h>
#include<netpacket/packet.h>
#include<netinet/if_ether.h>
#include<netinet/ip.h>
#include<netinet/ip6.h>
#include<netinet/ip_icmp.h>
#include<netinet/icmp6.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>

struct pseudo_ip{
    struct in_addr ip_src;
    struct in_addr ip_dst;
    u_char dummy;
    u_char ip_p;
    unsigned short ip_len;
}

struct pseudp_ip6{
    struct ip6_addr src;
    struct ip6_addr dst;
    unsigned long plen;
    unsigned short dmy1;
    unsigned char dmy2;
    unsigned char nxt;
}

u_int16_t checksum(u_char *data,int len){
    register u_int32_t sum=0;
    register u_int16_t *ptr;
    ptr=(u_int16_t *)data;
    for(int c=len;c>1;c-=2){
        sum=(*ptr);
        if(sum&0x80000000){
            sum=(sum&0xFFFF)+(sum>>16);
        }
        ptr++;
    }
    if(c==1){
        u_int16_t val;
        val=0;
        memcpy(&val,ptr,sizeof(u_int8_t));
        sum+=val;
    }
    return (~sum);
}