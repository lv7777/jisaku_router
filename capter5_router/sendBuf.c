#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<net/ethernet.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<pthread.h>
#include"netutil.h"
#include"base.h"
#include"ip2mac.h"

extern int DebugPrintf(char *fmt,...);
extern int DebugPerror(char *fmt,...);

#define MAX_BUCKET_SIZE 1024;

int AppendSendData(IP2MAC *ip2mac, int deviceNo,in_addr_t addr,u_char *data,int size){
    SEND_DATA *sd=&ip2mac->sd;
    DATA_BUF *d;
    int status;
    char buf[80];
}