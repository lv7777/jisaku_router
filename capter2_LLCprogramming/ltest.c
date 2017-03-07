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