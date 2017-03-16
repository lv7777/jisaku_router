// pcaptcpdump
#define TCPDUMP_MAGIC 0xa1b2c3d4
#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSiON_MINOR 4

#define DLT_EN10MB 1 //ethernet 10mb


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
#include<sys/time.h>


struct pcap_file_header{
    uint32_t magic;
    uint16_t major;
    uint16_t minor;
    int32_t timezone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t linktype;
};

struct pcap_pkheader{
    uint32_t ts_sec; 
    uint32_t ts_usec; 
    //struct timeval ts;
    uint32_t caplen;
    uint32_t len;
};

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
    FILE *fp=fopen("tes.pcap","wb");
    int magic=0xa1b2c3d4;
    struct pcap_file_header pfh;
    pfh.magic=magic;
    pfh.major=2;
    pfh.minor=4;
    pfh.timezone=9;
    pfh.sigfigs=0;
    pfh.snaplen=65535;
    pfh.linktype=1;
    if(fwrite(&pfh,sizeof(struct pcap_file_header),1,fp)){
        printf("ok!");
        
    }else{
        printf("error!!!");
    }
    //fprintf(fp,"%x",magic);
    int cnt=50;
    while(cnt){
        if((size=read(sd,buf,sizeof(buf)))<=0){
            perror("read");
        }else{
            printf("size is %x",size);
            struct pcap_pkheader pcappkt;
            //gettimeofday(&pcappkt.ts,NULL);
            pcappkt.ts_sec=0;
            pcappkt.ts_usec=50;
            pcappkt.caplen=size;
            pcappkt.len=size;
            if(fwrite(&pcappkt,sizeof(struct pcap_pkheader),1,fp)){
                printf("ok!");
            }else{
                printf("error!!!");
            }
            if(size>=sizeof(struct ether_header)){
                PrintEtherHeader( (struct ether_header *)buf,stdout );
            }else{
                fprintf(stderr,"read size(%d) 1, %d \n",size,sizeof(struct ether_header));
            }
        }
        cnt--;
    }
    printf("ok!end");
    fclose(fp);
    close(sd);

    return 0;
}