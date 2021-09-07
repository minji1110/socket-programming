#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXLINE 100

void error_handling(char *msg);

int main(int argc,char *argv[]){
    int sock_fd,n;
    char recvline[MAXLINE+1];
    struct sockaddr_in serv_addr;

    if(argc!=2){
        printf("usage : a.out <IPaddress>/");
        exit(1);
    }

    if((sock_fd=socket(AF_INET,SOCK_STREAM,0))<0)
        error_handling("socket error!");

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(13);
    
    if(inet_pton(AF_INET,argv[1],&serv_addr.sin_addr)<0){
        printf("inet_pton error for %s",argv[1]);
        exit(1);
    }

    if(connect(sock_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
        error_handling("connect error");

    while((n=read(sock_fd,recvline,MAXLINE))>0){
        recvline[n]=0;
        if(fputs(recvline,stdout)==EOF)
            error_handling("fputs error");
    }

    if(n<0) error_handling("read error");

    close(sock_fd);
    exit(0); 

}

void error_handling(char *msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}