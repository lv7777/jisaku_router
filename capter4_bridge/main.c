#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<poll.h>
#include<errno.h>
#include<signal.h>
#include<stdarg.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/if_ether.h>
#include"netutil.h"

//デバイスのインターフェースの名前とどこにデバッグするか。
typedef struct{
    char *device1;
    char *device2;
    char debugOut;
}CONF;

CONF arg={"wlan0","eth0",0};

typedef struct{
    int soc;
}DEV;
DEV device[2];

int EndFlag=0;

int DisableIpFoward(){
    FILE *fp;
    fp=open("/proc/sys/net/ipv4/ip_foward","w");
    if(fp){
        printf("error in open fp");
        return -1;
        fputs("0",fp);
        fclose(fp);
    }
    return 0;
}

void EndSignal(int sig){
    EndFlag=1;
}

//main function!!
int bridge(){
    u_char buf[2048];
int nready,i,size;
    while(EndFlag==0){
        switch(nready=poll(targets,2,100)){
            case -1:
                if(errno!=EINTR){
                    perror("poll");
                }
                break;

            case 0:
                break;

            default:
                for(i=0;i<2,i++){
                    if(targets[i].revents&(POLLIN|POLLERR)){
                        //ここ、deviceじゃなくてtargets[1].fdでもいいよな。。。
                        size=read(device[i].soc,buf,sizeof(buf));
                        if(size<=0){
                            printf("error");
                        }else{
                            if(AnalyzePacket(i,buf,size)!=-1){
                                size=write(device[(!i)]).soc;
                                if(size=<0)){
                                    perror("write");
                                }
                            }
                        }
                    }
                }
                break;
        }
    }

    return 0;
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

int AnalyzePacket(int devno,char *data,int size){
    u_char *ptr;
    int lest;
    struct ether_header *eh;
    ptr=data;
    lest=size;

    //error handle
    if(lest<sizeof(struct ether_header)){
        printf("error!!! packet size error!!");
        return -1;
    }

    eh=(struct ether_header *)data;
    ptr+=sizeof(struct ether_header);
    lest-=sizeof(struct ether_header);
    printf("[%d]",devno);
    PrintEtherHeader(eh,stderr);   
    


    return 0;
}

int main(){

    if((device[0].soc=InitRawSocket(p.device1,1,0))==-1){
        printf("error");
        return -1;
    }
    if((device[1].soc=InitRawSocket(p.device2,1,0))==-1){
        printf("error");
        return -1;
    }
    
    DisableIpFoward();

    signal(SIGINT,EndSignal);
    signal(SIGTERM,EndSignal);
    signal(SIGQUIT,EndSignal);

    printf("bridge start !!\n");

    bridge();

    printf("bridge end!!\n");

    close(device[0].soc);
    close(device[1].soc);


    return 0;
}
