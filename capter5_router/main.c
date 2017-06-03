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
    struct ether_header reh;
    struct iphdr rih;
    struct icmp icmp;
    u_char *ipptr;
    u_char *ptr,buf[1500];
    int len;

    memcpy(reh.ether_dhost,eh->ether_shost,6);
    memcpy(reh.ether_shost,DEVICE[DEVICE].hwaddr,6);
    rih.version=4;
    rih.ihl=20/4;
    rih.tos=0;
    rih.tot_len=htons(sizeof(struct icmp)+64);
    rih.id=0;
    rih.flag_off=0;
    rih.ttl=64;
    rih.protocol=IPPROTO_ICMP;
    rih.check=0;
    rih.saddr=DEVICE[deviceNo].addr.s_addr;
    rih.daddr=iphdr->saddr;

    rih.check=checksum((u_char *)&rih,sizeof(struct iphdr));

    icmp.icmp_type=ICMP_TIME_EXCEEDED;
    icmp.icmp_code=IMCP_TIMXCEED_INTRANS;
    icmp.icmp_cksum=0;
    icmp.icmp_void=0;

    ipptr=data+sizeof(struct ether_header);

    icmp.icmp_cksum=checksum2((u_char *)&icmp,8,ipptr,64);

    ptr=buf;
    memcpy(ptr,&rih,sizeof(struct ether_header));
    ptr+=sizeof(struct iphdr);
    memcpy(ptr,&icmp,8);
    ptr+=8;
    memcpy(ptr,ipptr,64);
    ptr+=64;
    len=ptr-buf;

    DebugPrintf("write:SendIcmpTimeExceeded:[%d] %dbytes\n",deviceNo,len);

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


    //macaddrがifと同じものだけ受け取ることでブロードキャストとか弾く。
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
       if(arp->arp_op==htons(ARPOP_REQUEST)){
            DebugPrintf("[%d]recv:ARP REPLY:%dbytes\n",deviceNo,size);
            Ip2Mac(deviceNo,*(in_addr_t *)arp->arp_spa,arp->arp_sha);
        }
        if(arp->arp_op==htons(ARPOP_REPLY)){
            DebugPrintf("[%d]:recv:ARP REPLY%dbytes\n",deviceNo,size);
            Ip2Mac(deviceNo,*(in_addr_t *)arp->spa,arp->arp_sha);
        }
    }else if(ntohs(eh->ether_type)==ETHERTYPE_IP){
        struct iphdr *iphdr;
        u_char option[1500];
        int optionlen;


        if(lest<sizeof(struct iphdr)){
            DebugPrintf("%d:lest(%d)<sizeof(struct iphdr)\n",deviceNo,lest);
            return -1;
        }

        iphdr=(struct iphdr *)ptr;
        ptr+=sizeof(struct iphdr);
        lest-=sizeof(struct iphdr);

        optionLen=iphdr->ihl*4-sizeof(struct iphdr);
        if(optionLen>=1500){
            DebugPrintf("%d:IP header optionlen(%d) is too big!",deviceNo,optionLen);
            return -1;
        }

        memcpy(option,ptr,optionLen);
        ptr+=optionLen;
        lest-=optionLen;

        if(checkIPchecksum(iphdr,option,optionLen)==0){
            DebugPrintf("[%d]:bad ip checksum\n",deviceNo);
            return -1;
        }

        if(iphdr->ttl-1==0){
            Debugprintf("[%d]:iphdr->ttl == 0 error\n",deviceNo);
            SendIcmpTimeExceeded(deviceNo,eh,iphdr,data,size);
            return -1;
        }
        tno=(!deviceNo);
        //netmask 255.255.255.0
        //subnet  192.168.88.0
        if((iphdr->daddr&Device.[tno].netmask.s_addr)==Device[tno].subnet.s_addr){
            IP2MAC *Ip2mac;
            DebugPrintf("[%d]recv:%s is Target Segment\n",deviceNo,in_addr_t2str(iphdr->daddr,buf,sizeof(buf)));

            if(iphdr->daddr==Device[tno].addr.s_addr){
                DebugPrintf("[%d]:recv:myaddr \n",deviceNo);
                return 1;
            }

            ip2mac=Ip2Mac(tno,iphdr->daddr,NULL);
            if(ip2mac->flag==FLAG_NG||ip2mac->sd.dno!==0){
                DebugPrintf("[%d]:Ip2Mac:error or sending\n",deviceNo);
                AppendSendData(ip2mac,1,iphdr->daddr,data,size);
                return -1;
            }else{
                memcpy(hwaddr,ip2mac->hwaddr,6);
            }

        }else{
            //上位ルーターに送る。
            IP2MAC *ip2mac;
            DebugPrintf("[%d]:%s to sending next router\n",deviceNo,in_addr_t2str(iphdr->daddr,buf,sizeof(buf)));
            ip2mac=Ip2Mac(tno,NextRouter.s_addr,NULL);
            if(ip2mac->flag==FLAG_NG||ip2mac->sd.dno!=0){
                DebugPrintf("[%d]:ip2mac:error or sending\n",deviceNo);
                AppendSendData(ip2mac,1,NextRouter.s_addr,data,size);
                return -1;
            }else{
                memcpy(hwaddr,ip2mac->hwaddr,6);
            }
        }

        memcpy(eh->ether_dhost,hwaddr,6);
        memcpy(eh->ether_shost,Device[tno].hwaddr,6);

        iphdr->ttl--;
        iphdr->check=0;
        iphdr->checksum2((u_char *),sizeof(struct iphdr),option,optionLen);

        write(Device[tno].soc,data,size);

    }
    return 0;
}