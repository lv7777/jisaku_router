#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include<sys/ioctl.h>

#include<arpa/inet.h>
#include<sys/socket.h>
#include<linux/if.h>
#include<net/ethernet.h>
#include<netpacket/packet.h>
#include<netinet/if_ether.h>
#include<netinet/ip.h>
#include<netinet/ip6.h>
#include<netinet/ip_icmp.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include"checksum.h"
#include"print.h"

#ifndef ETHERTYPE_IPV6
#define ETHERTYPE_IPV6 0x86dd
#endif

int AnalyzeArp(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct ether_arp *arp;
    ptr=data;
    lest=size;
    if(lest<sizeof(struct ether_arp)){
        fprintf(stderr,"lest(%d)<sizeof(struct iphdr)\n",lest);
        return -1;
    }
    arp=(struct ether_arp*)ptr;

    //TODO:what this code?
    ptr+=sizeof(struct ether_arp);
    lest+=sizeof(struct ether_arp);

    PrintArp(arp,stdout);

    return 0;
}


int AnalyzeIcmp(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct icmp *icmp;
    ptr=data;
    lest=size;
    if(lest<sizeof(struct icmp)){
        fprintf(stderr,"lest(%d)<sizeof(struct iphdr)\n",lest);
        return -1;
    }
    icmp=(struct icmp*)ptr;

    //TODO:what this code?
    ptr+=sizeof(struct icmp);
    lest+=sizeof(struct icmp);

    PrintIcmp(icmp,stdout);

    return 0;
}

int AnalyzeIcmp6(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct icmp6_hdr *icmp6;
    ptr=data;
    lest=size;
    if(lest<sizeof(struct  icmp6_hdr*)){
        fprintf(stderr,"lest(%d)<sizeof(struct icmp6hdr)\n",lest);
        return -1;
    }
    icmp6=(struct icmp6_hdr*)ptr;

    //TODO:what this code?
    ptr+=sizeof(struct icmp6_hdr*);
    lest+=sizeof(struct icmp6_hdr*);

    PrintIcmp(icmp6,stdout);

    return 0;
}


int AnalyzeTcp(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct tcphdr *tcphdr;
    ptr=data;
    lest=size;
    if(lest<sizeof(struct tcphdr)){
        fprintf(stderr,"lest(%d)<sizeof(struct tcphdr)\n",lest);
        return -1;
    }
    tcphdr=(struct tcphdr*)ptr;

    //TODO:what this code?
    ptr+=sizeof(struct tcphdr);
    lest+=sizeof(struct tcphdr);

    PrintTcp(tcphdr,stdout);

    return 0;
}


int AnalyzeUdp(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct udphdr *udphdr;
    ptr=data;
    lest=size;
    if(lest<sizeof(struct udphdr)){
        fprintf(stderr,"lest(%d)<sizeof(struct udphdr)\n",lest);
        return -1;
    }
    udphdr=(struct udphdr*)ptr;

    //TODO:what this code?
    ptr+=sizeof(struct udphdr);
    lest+=sizeof(struct udphdr);

    PrintUdp(udphdr,stdout);

    return 0;
}


int AnalyzeIp(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct iphdr *iphdr;
    u_char *option;
    int optionlen,len;
    unsigned short sum;
    ptr=data;
    lest=size;
    if(lest<sizeof(struct iphdr)){
        fprintf(stderr,"lest(%d)<sizeof(struct iphdr)\n",lest);
        return -1;
    }
    iphdr=(struct iphdr*)ptr;

    //TODO:what this code?
    ptr+=sizeof(struct iphdr);
    lest+=sizeof(struct iphdr);

    optionlen=iphdr->ihl*4-sizeof(struct iphdr);
    if(optionlen>0){
        if(optionlen>=1500){
            fprintf(stderr,"IP optionlen");
            return -1;
        }
        option=ptr;
        ptr+=optionlen;
        lest+=optionlen;
    }

    if(checkIPchecksum(iphdr,option,optionlen)==0){
        fprintf(stderr,"bad ip checksum");
        return -1;
    }

    PrintIpHeader(iphdr,option,optionlen,stdout);

    if(iphdr->protocol==IPPROTO_ICMP){
        len=ntohs(iphdr->tot_len)-iphdr->ihl*4;
        sum=checksum(ptr,len);
        if(sum!=0&&sum!=0xFFFF){
            fprintf(stderr,"bad icmp checksum\n");
            return -1;
        }
        AnalyzeIcmp(ptr,lest);
    }else if(iphdr->protocol==IPPROTO_TCP){
        len=ntohs(iphdr->tot_len)-iphdr->ihl*4;
        if(checkIPDATAchecksum(iphdr,ptr,len)==0){
            fprintf(stderr,"bad tcp checksum\n");
            return -1;
        }
        AnalyzeTcp(ptr,lest);
    }else if(iphdr->protocol==IPPROTO_UDP){
        struct udphdr *udphdr;
        udphdr=(struct udphdr*)ptr;
        len=ntohs(iphdr->tot_len)-iphdr->ihl*4;
        if(udphdr->check!=0&&checkIPDATAchecksum(iphdr,ptr,len)==0){
            fprintf(stderr,"bad udp checksum");
            return -1;
        }
        AnalyzeUdp(ptr,lest);
    }

    return 0;
}


int AnalyzeIpv6(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct ip6_hdr *ip6;
    int len;

    ptr=data;
    lest=size;

    if(lest<sizeof(struct ip6_hdr)){
        fprintf(stderr,"lest(%d)<sizeof(struct ip6_hdr)\n",lest);
        return -1;
    }
    ip6=(struct ip6_hdr *)ptr;
    ptr+=sizeof(struct ip6_hdr);
    lest+=sizeof(struct ip6_hdr);
    PrintIp6Header(ip6,stdout);

    if(ip6->ip6_nxt==IPPROTO_ICMPV6){
        len=ntohs(ip6->ip6_plen);
        if(checkIP6DATAchecksum(ip6,ptr,len)==0){
            fprintf(stderr,"bad icmpv6 checksum");
            return -1;
        }
        AnalyzeIcmp6(ptr,lest);
    }else if(ip6->ip6_nxt==IPPROTO_TCP){
        len=ntohs(ip6->ip6_plen);
        if(checkIP6DATAchecksum(ip6,ptr,len)==0){
            fprintf(stderr,"bad tcp6 checksum");
            return -1;
        }
        AnalyzeTcp(ptr,lest);
    }else if(ip6->ip6_nxt=IPPROTO_UDP){
        len=ntohs(ip6->ip6_plen);
        if(checkIP6DATAchecksum(ip6,ptr,len)==0){
            fprintf(stderr,"bad udp6 checksum");
            return -1;
        }
        AnalyzeUdp(ptr,lest);
    }
    return 0;
}

int AnalyzePacket(u_char *data,int size){
    u_char *ptr;
    int lest;
    struct ether_header *eh;

    ptr=data;
    lest=size;

    if(lest<sizeof(*eh)){
        fprintf(stderr,"lest(%d)<sizeof(ether_header)\n",lest);
    }

    eh=(struct ether_header*)ptr;
    ptr+=sizeof(*eh);
    lest-=sizeof(*eh);

    if(ntohs(eh->ether_type)==ETHERTYPE_ARP){
        fprintf(stderr,"Packet[%dbytes]\n",size);
        PrintEtherHeader(eh,stdout);
        AnalyzeArp(ptr,lest);
    }else if(ntohs(eh->ether_type)==ETHERTYPE_IP){
        fprintf(stderr,"Packet[%dbytes]\n",size);
        PrintEtherHeader(eh,stdout);
        AnalyzeIp(ptr,lest);
    }else if(ntohs(eh->ether_type)==ETHERTYPE_IPV6){
        fprintf(stderr,"Packet[%dbytes]\n",size);
        PrintEtherHeader(eh,stdout);
        AnalyzeIpv6(ptr,lest);
    }

    return 0;
}