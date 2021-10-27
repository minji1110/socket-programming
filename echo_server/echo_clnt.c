#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);
void str_cli(FILE *fp, int sockfd);
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
    int sockfd;
	struct sockaddr_in serv_addr;
	
	if(argc!=3){
		printf("Usage : %s <Ip> <Port>\n",argv[0]);
		exit(1);
	}

    //클라이언트 소켓 주소 할당
	sockfd=socket(PF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
		error_handling("socket() error");
	
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

    //서버에 연결 요청
	if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("connect() error");
        
    else {
        printf("connect() return!\n");
        str_cli(stdin, sockfd);
    }

    exit(0);
}

//fgets로 sendline에 문자열 읽고(사용자입력), 읽은것을 서버에 write.
//readline으로 recvline에 서버에서 온 문자열을 읽고, fputs로 화면에 출력.
void str_cli(FILE *fp, int sockfd){
    int n;
    char sendline[BUF_SIZE];
    char recvline[BUF_SIZE];

    while(1){
        fputs("메세지 입력 : ",stdout);
        
        char * input=fgets(sendline,BUF_SIZE,fp);
        if(input==NULL) break;

        write(sockfd, sendline, strlen(sendline));
        if((n=read(sockfd, recvline, BUF_SIZE-1))==0)
            error_handling("str_sli() error, server terminated prematurely! ");
        
        recvline[n]=0;
        fputs(recvline,stdout);
    }
}

void error_handling(char *msg){
	perror(msg);
	fputc('\n',stderr);
	exit(1);
}