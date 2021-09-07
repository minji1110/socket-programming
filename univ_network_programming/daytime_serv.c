#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#define MAXLINE 30

int main(int argc,char *argv[]){
    int listen_fd;    
    int conn_fd;
    struct sockaddr_in serv_addr;
    char buff[MAXLINE];
    time_t ticks;

    listen_fd=socket(AF_INET,SOCK_STREAM,0);

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(13);

    if(bind(listen_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handling("bind() error");

    for(;;){
        conn_fd=accept(listen_fd,(struct sockaddr*) NULL,NULL);

        ticks=time(NULL);
        snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
        write(conn_fd,buff,strlen(buff));

        close(conn_fd);
    }   
}

void error_handling(char *msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}