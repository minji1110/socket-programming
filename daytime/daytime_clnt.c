#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXLINE 100

void error_handling(char *msg);

int main(int argc,char *argv[]){
    int sock_fd,n;
    char recvline[MAXLINE];
    struct sockaddr_in serv_addr;

    if(argc!=3)
        error_handling("usage : a.out <IPaddress>/");
    

    if((sock_fd=socket(AF_INET,SOCK_STREAM,0))<0)
        error_handling("socket error!");
    else puts("소켓 연결");

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
    // serv_addr.sin_port=htons(13);
    
    if(inet_pton(AF_INET,argv[1],&serv_addr.sin_addr)<=0){
        printf("inet_pton error for %s",argv[1]);
        exit(1);
    }
    else puts("정상 주소..");

    if(connect(sock_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handling("connect error");
    else 
        puts("서버 연결 완료...");

    while((n=read(sock_fd,recvline,MAXLINE-1))>0){
        recvline[n]=0;
        if(fputs(recvline,stdout)==EOF)
            error_handling("fputs error");
        printf("%s",recvline);
    }

    if(n<0) error_handling("read error");

    // close(sock_fd);
    exit(0); 

}

void error_handling(char *msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}