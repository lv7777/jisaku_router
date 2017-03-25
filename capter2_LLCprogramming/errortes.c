#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

int main(){
    int s;
    s=socket(3000,4000,5000);
    perror("error");
}