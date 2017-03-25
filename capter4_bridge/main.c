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
