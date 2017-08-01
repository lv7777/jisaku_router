#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<linux/if.h>
#include<net/netinet.h>
#include<netinet/ip.h>
#include<netpacket/packet.h>
#include<netinet/if_ether.h>

extern int DebugPrintf(char *fmt,...);
extern int DebugPerror(char *msg);

//init raw socket
int InitRawSocket(char *device,int promiscFlag,int ipOnly){
    struct ifreq ifreq;
    struct sockeaddr_ll sa;

    int soc;
    if(ipOnly){
        if(socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP))<0){
            DebugPerror("socket");
            return -1;
        }
    }else{
        if((soc=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0){
            DebugPerror("socket");
            return -1;
        }
    }

	memset(&ifreq,0,sizeof(struct ifreq));
	strncpy(ifreq.ifr_name,device,sizeof(ifreq.ifr_name)-1);
    if(ioctl(soc,SIOCGIFINDEX,&ifreq)<0){
		DebugPerror("ioctl");
		close(soc);
		return(-1);
	}
    sa.sll_family=PF_PACKET;

    if(ipOnly){
        sa.sll_protocol=htons(ETH_P_IP);
    }else{
        sa.sll_protocol=htons(ETH_P_ALL);
    }
    sa.sll_ifindex=ifreq.ifr_ifindex;
    if(bind(soc,(struct sockaddr *)&sa,sizeof(sa))<0){
        DebugPerror("bind");
        close(soc);
        return -1;
    }

    if(promiscFlag){
        if(ioctl(soc,SIOCGIFFLAGS,&ifreq)<0){
            DebugPerror("ioctl");
            close(soc);
            return -1;
        }
        ifreq.ifr_flags=ifreq.ifr_flags|IFF_PROMISC;
        if(ioctl(soc,SIOCSIFFLAGS,&ifreq)<0){
            DebugPerror("ioctl");
            close(soc);
            return -1;
        }
    }
    return soc;
}

int GetDeviceInfo(char *device,u_char hwaddr[6],struct in_addr *uaddr,struct in_addr *subnet,struct in_addr *mask){
    struct ifreq ifreq;
    struct sockaddr_in addr;
    int soc;
    u_char *p;
    
    if(soc=socket(PF_INET,SOCK_DGRAM,0)<0){
        DebugPerror("socket");
        return -1;
    }

    memset(&ifreq,0,sizeof(struct ifreq));
    strncpy(ifreq.ifr_name,device,sizeof(ifreq.ifr_name));

    if(ioctl(soc,SIOCGIFHWADDR,&ifreq)==-1){
        DebugPerror("ioctl");
        close(soc);
        return -1;
    }else{
        p=(u_char *)&ifreq.ifr_hwaddr.sa_data;
        memcpy(hwaddr,p,6);
    }

    if(ioctl(soc,SIOCGIFADDR,&ifreq)==-1){
        DebugPerror("ioctl");
        close(soc);
        return -1;
    }else if(ifreq.ifr_name.sa_family!=PF_INET){
        DebugPerror("%s is not PF_INET\n",device);
        close(soc);
        return -1;
    }else{
        memcpy(&addr,&ifreq.ifr_name,sizeof(struct sockaddr_in));
        *uaddr=addr.sin_addr;
    }

    if(ioctl(soc,SIOCGIFNETMASK,&ifreq)==-1){
        DebugPerror("ioctl");
        close(soc);
        return -1;   
    }else{
        memcpy(&addr,&ifreq.ifr_addr,sizeof(struct sockaddr_in));
        *mask=addr.sin_addr;
    }

    subnet->s_addr=((uaddr->s_addr)&(mask->s_addr));

    close(soc);
    return 0;

}



char* my_ether_ntoa_r(u_char *hwaddr,char *buf,socklen_t size){
    snprintf(buf,size,"%02x:%02x:%02x:%02x:%02x:%02x",hwaddr[0],hwaddr[1],hwaddr[2],hwaddr[3],hwaddr[4],hwaddr[5]);
    return buf;
}

char *my_inet_ntoa_r(struct in_addr *addr,char *buf,socklen_t size){
    inet_ntop(PF_INET,addr,buf,size);
    return buf;
}

char *in_addr_t2str(in_addr_t addr,char *buf,socklen_t size){
    struct in_addr a;
    a.s_addr=addr;
    inet_ntop(PF_INET,addr,buf,size);
    return buf;
}

int PrintEtherHeader(struct ether_header *eh,FILE *fp){
    char buf[80];
    fprintf(fp,"ether_header--------------------------\n");
    fprintf(fp,"ether_dhost=%s\n",my_ether_ntoa_r(eh->ether_dhost,buf,sizeof(buf)));
    fprintf(fp,"ether_shost=%s\n",my_ether_ntoa_r(eh->ether_shost,buf,sizeof(buf)));
    fprintf(fp,"ether_type=%02X",ntohs(eh->ether_type));
    switch(htons(eh->ether_type)){
        case ETH_P_IP:
        fprintf(fp,"(IP)\n");
        break;
        case ETH_P_IPV6:
        fprintf(fp,"(IPv6)\n");
        break;
        case ETH_P_ARP:
        fprintf(fp,"(ARP)\n");
        break;
        default:
        fprintf(fp,"(unknown)\n");
    }
    return 0;
}


u_int16_t checksum(u_char *data,int len)
{
register u_int32_t       sum;
register u_int16_t       *ptr;
register int     c;

        sum=0;
        ptr=(u_int16_t *)data;

        for(c=len;c>1;c-=2){
                sum+=(*ptr);
                if(sum&0x80000000){
                        sum=(sum&0xFFFF)+(sum>>16);
                }
                ptr++;
        }
        if(c==1){
                u_int16_t       val;
                val=0;
                memcpy(&val,ptr,sizeof(u_int8_t));
                sum+=val;
        }

        while(sum>>16){
                sum=(sum&0xFFFF)+(sum>>16);
        }

	return(~sum);
}



u_int16_t checksum2(u_char *data1, int len1, u_char *data2, int len2) {
  u_int32_t sum = 0;
  u_int16_t *ptr;
  int c;
  ptr = (u_int16_t *)data1;
  for (c = len1; c > 1; c -= 2) {
    sum += (*ptr);
    if (sum & 0x80000000) {
      sum = (sum & 0xffff) + (sum >> 16);
    }
    ptr++;
  }
  if (c == 1) {
    /* padding */
    u_int16_t val;
    val = ((*ptr) << 8) + (*data2);
    sum += val;
    if (sum & 0x80000000) {
      sum = (sum & 0xffff) + (sum >> 16);
    }
    ptr = (u_int16_t *)(data2 + 1);
    len2--;
  }
  else {
    ptr = (u_int16_t *)data2;
  }
  for (c = len2; c > 1; c -= 2) {
    sum += (*ptr);
    if (sum & 0x80000000) {
      sum = (sum & 0xffff) + (sum >> 16);
    }
    ptr++;
  }
  if (c == 1) {
    u_int16_t val = 0;
    memcpy(&val, ptr, sizeof(u_int8_t));
    sum += val;
  }
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  return ~sum;
}


int checkIPchecksum(struct iphdr *iphdr,u_char *option,int optionLen)
{
unsigned short	sum;

	if(optionLen==0){
		sum=checksum((u_char *)iphdr,sizeof(struct iphdr));
		if(sum==0||sum==0xFFFF){
			return(1);
		}
		else{
			return(0);
		}
	}
	else{
		sum=checksum2((u_char *)iphdr,sizeof(struct iphdr),option,optionLen);
		if(sum==0||sum==0xFFFF){
			return(1);
		}
		else{
			return(0);
		}
	}
}



typedef struct {
    struct ether_header eh;
    struct ether_arp arp;
}PACKET_ARP;

int SendArpRequestB(int soc,in_addr_t,target_ip,){
    PACKET_ARP arp;
    int total;
    u_char *p;
    u_char buf[sizeof(struct ether_header)+sizeof(struct ether_arp)];

    union{
        unsigned long l;
        u_char c[4];
    }lc;
    int i;

    arp.arp.arp_hrd=htons(ARPHRD_ETHER);
    arp.arp.arp_pro=htons(ETHERTYPE_IP);
    arp.arp.arp_hln=6;
    arp.arp.arp_pln=4;
    arp.arp.arp_op=htons(ARPOP_REQUEST);

    for(i=0;i<6;i++){
        arp.arp.arp_sha[i]=my_mac[i];
    }

    lc.l=my_ip;
    for(i=0;i<6;i++){
        arp.arp.arp_spa[i]=lc.c[i];
    }

    lc.l=target_ip;
    for(i=0;i<6;i++){
        arp.arp.arp_tpa[i]=lc.c[i];
    }

    for(i=0;i<6;i++){
        arp.eh.ehter_dhost[i]=target_mac[i];
    }

    for(i=0;i<6;i++){
        arp.eh.ehter_shost[i]=my_mac[i];
    }

    arp.eh.ether_type=htons(ETHERTYPE_ARP);

    memset(buf,0,sizeof(buf));
    p=buf;
    memcpy(p,&arp.eh,sizeof(struct ether_header));
    p+=sizeof(struct ether_header);
    memcpy(p,&arp.arp,sizeof(struct ether_arp));
    p+=sizeof(struct ether_arp);

    total=p-buf;
DebugPrintf("total is %d",total);
    write(soc,buf,total);
    
    return 0;

}


// int checkIPDATAchecksum(struct iphdr *iphdr,unsigned char *data,int len)
// {
// struct pseudo_ip        p_ip;
// unsigned short  sum;

//         memset(&p_ip,0,sizeof(struct pseudo_ip));
//         p_ip.ip_src.s_addr=iphdr->saddr;
//         p_ip.ip_dst.s_addr=iphdr->daddr;
//         p_ip.ip_p=iphdr->protocol;
//         p_ip.ip_len=htons(len);

//         sum=checksum2((unsigned char *)&p_ip,sizeof(struct pseudo_ip),data,len);
//         if(sum==0||sum==0xFFFF){
//                 return(1);
//         }
//         else{
//                 return(0);
//         }
// }
