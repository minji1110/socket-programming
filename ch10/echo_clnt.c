//에코 클라이언트
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
    int sock;
	struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    int str_len;
	
	if(argc!=3){
		printf("Usage : %s <Ip> <Port>\n",argv[0]);
		exit(1);
	}

    //클라이언트 소켓 주소 할당
	sock=socket(PF_INET,SOCK_STREAM,0);
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

    //서버에 접속 요청
	if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("connect() error");
    else 
        puts("서버 연결 완료...");

    while(1){
        fputs("메세지를 입력하세요(종료하려면 Q(q) 입력) : ",stdout);
        fgets(message,BUF_SIZE,stdin);

        //q(Q) 입력 시 종료
        if(!strcmp(message,"q\n")||!strcmp(message,"Q\n"))
            break;
        
        write(sock,message,strlen(message));
        str_len=read(sock,message,BUF_SIZE-1);
        message[str_len]=0;
    }
    close(sock);
    return 0;
}

void error_handling(char *msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}