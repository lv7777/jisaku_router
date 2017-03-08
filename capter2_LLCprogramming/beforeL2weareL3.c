#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(){
    int serverSocket;
    int clientSocket;
    serverSocket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    struct sockaddr_in client;
    int len;

    addr.sin_family=AF_INET;
    addr.sin_port=htons(12345);
    addr.sin_addr.s_addr=INADDR_ANY;
    bind(serverSocket,(struct sockaddr*)&addr,sizeof(addr));
    listen(serverSocket,5);

    len=sizeof(client);
    clientSocket=accept(serverSocket,(struct sockaddr*)&client,&len);
    write(clientSocket,"HELLO",5);
    close(clientSocket);
    close(serverSocket);
}