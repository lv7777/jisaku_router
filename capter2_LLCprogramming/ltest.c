// ltest.c link layer test.

#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<sys/socket.h>

//if you want to handle link layer packet, you will need follow headers
#include<linux/if.h>
#include<netpacket/packet.h>
#include<netinet/if_ether.h>

int socketinit(char *device,int promiscasmode,int iponly){
    struct ifreq ifreq;
    struct sockaddr_ll sa;

//file discriptor is created by socket function
int sd;
//when handle linklayer, we use PF_PACKET
//when handle linklayer, we use SOCK_RAW, other than SOCK_STREAM,SOCK_DGRAM
sd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP));
if(sd<0){
    printf("err!!!");
    return 1;
}

memset(&ifreq,0,sizeof(struct ifreq));
strncpy(ifreq.ifr_name,device,sizeof(ifreq.ifr_name)-1);
if(ioctl(sd,SIOCGIFINDEX,&ifreq)<0){
    perror("ioctl");
    close(sd);
    return 1;
}

sa.sll_family=PF_PACKET;
sa.sll_protocol=htons(ETH_P_IP);
sa.sll_ifindex=ifreq.ifr_ifindex;
if(bind(sd,(struct sockaddr *)&sa,sizeof(sa))<0){
    perror("bind");
    close(sd);
    return 1;
}
//if promiscasmode
if(ioctl(sd,SIOCGIFFLAGS,&ifreq)<0){
    perror("ioctl");
    close(sd);
    return 1;
}
ifreq.ifr_flags=ifreq.ifr_flags | IFF_PROMISC;

if(ioctl(sd,SIOCSIFFLAGS,&ifreq)<0){
    perror("ioctl");
    close(sd);
    return 1;
}

return (sd);
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

int main(int argc,char **argv,char **envp){
    int sd,size;
    u_char buf[2048];
    if(argc<=1){
        fprintf(stderr,"ltest device-name\n");
        return 1;
    }

    if( (sd=socketinit(argv[1],0,0))==-1 ){
        fprintf(stderr,"socketinit:err:%s\n",argv[1]);
        return 1;
    }

    while(1){
        if((size=read(sd,buf,sizeof(buf)))<=0){
            perror("read");
        }else{
            if(size>=sizeof(struct ether_header)){
                PrintEtherHeader( (struct ether_header *)buf,stdout );
            }else{
                fprintf(stderr,"read size(%d) 1, %d \n",size,sizeof(struct ether_header));
            }
        }
    }

    close(sd);

    return 0;
}