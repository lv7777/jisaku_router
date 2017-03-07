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

//file discriptor is created by socket function
int sd;
//when handle linklayer, we use PF_PACKET
//when handle linklayer, we use SOCK_RAW, other than SOCK_STREAM,SOCK_DGRAM
sd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP));
if(sd<0){
    printf("err!!!");
}
return 1;
}

int main(int argc,char **argv){
    return 0;
}