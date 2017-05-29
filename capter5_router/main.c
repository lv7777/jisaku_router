#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>
#include<ip/addr.h>
#include<arpa/inet.h>
#include<poll.h>
#include<errno.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/if_ether.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<net/pthread.h>

typedef struct{
    char *Device1;
    char *Device2;
    int DebugOut;
    char *NextRouter;//!!!!
}PARAM;

PARAM param ={"eth1","eth2",0,"192.168.88.1"};

struct in_addr NextRouter;
DEVICE device[2];
int EndFlag=0;

int DebugPrintf(char *fmt){
    if(param.DebugOut){
        va_list args;
        va_start(args,fmt);
        vsprintf(stderr,fmt,args);
        va_end(args);
    }
    return 0;
}

int DebugPerror(char *msg){
    if(param.DebugOut){
        fprintf(stderr,"%s : %s\n",msg,strerror(errno));
    }
    return 0;
}

int SendIcmpTimeExceeded(int deviceNo,struct ether_header *eh,struct iphdr *iphdr,u_char *data,int size){


    return 0;
}

int AnalyzePacket(int deviceNo,u_char *data,int size){
    u_char *ptr;
    int lest;
    struct ether_header *eh;
    char buf[80];
    int tno;
    u_char hwaddr[6];

    ptr=data;
    lest=size;
    if(lest<sizeof(struct ehter_header)){
        DebugPrintf("[%d]:lest(%d)<sizeof(struct ehter_header)\n",deviceNo,lest);
        return -1;
    }
    eh=(struct ether_header *)ptr;
    ptr+=sizeof(struct ehter_header);
    lest-=sizeof(struct ether_header);

    if(memcmp(&eh->ether_dhost,device[deviceNo].hwaddr,6)!=0){
        DebugPrintf("[%d]:dhost not match %s\n",deviceNo,my_ether_ntoa_r((u_char *)&eh->ether_dhost,buf,sizeof(buf)));
        return -1;
    }
    if(ntohs(eh->ether_type)==EHTHERTYPE_ARP){
        struct ether_arp *arp;
        if(lest<sizeof(struct ether_arp)){
            DebugPrintf("[%d]:lest(%d)<sizeof(struuct ether_arp)\n",deviceNo,lest);
            return -1;
        }
        arp+=(struct ether_arp *)ptr;
        ptr+=sizeof(struct ether_arp);
        lest-=sizeof(struct ether_arp);


        if(arp->arp_op==htons())
    }
}