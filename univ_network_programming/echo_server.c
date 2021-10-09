#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 30
void error_handling(char *msg);
void str_echo(int connfd);

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clientlen;
    struct sockaddr_in servaddr,clntaddr;

    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    listenfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(atoi(argv[1]));

    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1)
        error_handling("bind() error");
    else printf("bind() return!\n");
    
    if(listen(listenfd,5)<0)
        error_handling("listen() error");
    else printf("listen() return!\n");

    for(;;){
        clientlen=sizeof(clntaddr);
        connfd=accept(listenfd,(struct sockaddr*) &clntaddr,&clientlen);
        
        if(connfd==-1)
		    error_handling("accept() error");
        else printf("accept() return!\n");

        //자식 프로세스
        if((childpid=fork())==0){
            close(listenfd);
            str_echo(connfd);
            close(connfd);

            fputs("클라이언트 연결 해제 완료!",stdout);
            exit(0);

        }
        //부모 프로세스
        close(connfd);
    } 
    close(listenfd);
    return 0;
}

void str_echo(int connfd){
    ssize_t n;
    char buf[100];

    again:
        while((n=read(connfd,buf,BUF_SIZE))>0) {
            //받은 문자열을 그대로 전송
            write(connfd, buf, n);
        }

        if(n<0 && errno==EINTR) goto again;
        else if(n<0) error_handling("read() error");
}

void error_handling(char *msg){
    perror(msg);
	// fputc('\n',stderr);
	exit(1);
}