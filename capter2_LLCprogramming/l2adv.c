//l3 programming

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(){
    int s;
    s=socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        perror("sock failed");
        return 1;
    }
    return 0;
}