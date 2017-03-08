#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(){
    int s;
    int n;
    char buf[80];
    struct sockaddr_in addr;
    s=socket(AF_INET,SOCK_STREAM,0);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(12345);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    connect(s,(struct sockaddr*)&addr,sizeof(addr));

    memset(buf,0,sizeof(buf));
    n=read(s,buf,sizeof(buf));
    printf("%d,%s\n",n,buf);
    close(s);
}
