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
    close(fd);
    return 1;
}
ifreq.ifr_flags=ifreq.ifr_flags | IFF_PROMISC;

if(ioctl(sd,SIOCSIFFLAGS,&ifreq)<0){
    perror("ioctl");
    close(fd);
    return 1;
}

return (sd);
}

char* my_ether_ntoa_r(u_char *hwaddr,char *buf,socketlen_t size){
    snprintf(buf,size,"%02x:%02x:%02x:%02x:%02x:%02x",hwaddr[0],hwaddr[1],hwaddr[2],hwaddr[3],hwaddr[4],hwaddr[5]);
    return buf;
}

int main(int argc,char **argv){
    return 0;
}