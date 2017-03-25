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

int InitRawSocket(char *device,int promisecFlag, int ipOnly){
    int sd;
    struct sockaddr_ll sa;
    struct ifreq ifreq;
    sd=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_IP));
    if(sd<0){
        perror("socket");
        return -1;
    }
    memset(&ifreq,0,sizeof(ifreq));
    strncpy(ifreq.ifr_name,device,sizeof(ifreq.ifr_name)-1);
    if(ioctl(sd,SIOCGIFINDEX,&ifreq)<0){
        perror("ioctl");
        return -1;
    }
    sa.sll_family=PF_PACKET;
    sa.sll_protocol=htons(ETH_P_IP);
    sa.sll_ifindex=ifreq.ifr_ifindex;
    if(bind(sd,(struct sockaddr *)&sa,sizeof(sa))<0){
        perror("bind");
        close(sd);
        return -1;
    }
    return sd;
}

char* my_ether_ntoa_r(u_char *hwaddr,char *buf,socklen_t size){
    snprintf(buf,size,"%02x:%02x:%02x:%02x:%02x:%02x",hwaddr[0],hwaddr[1],hwaddr[2],hwaddr[3],hwaddr[4],hwaddr[5]);
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
