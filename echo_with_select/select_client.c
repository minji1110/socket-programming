#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

void error_handling(char *message);
void str_cli(FILE *fp, int sockfd);
#define BUF_SIZE 10

int main(int argc, char *argv[]){
    int sockfd;
	struct sockaddr_in serv_addr;
	
	if(argc!=3){
		printf("Usage : %s <Ip> <Port>\n",argv[0]);
		exit(1);
	}

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		error_handling("socket() error");
	
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	if(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("connect() error");
        
    else{
        // int fd=open("text.txt",O_CREAT|O_RDONLY);
        // if(fd==-1) error_handling("file open() error");
        // str_cli(fd, sockfd);
        str_cli(stdin, sockfd);
    }    
    
    exit(0);
}

void str_cli(FILE *fp, int sockfd){
    int max_fd, stdin_eof,n;
    fd_set read_set;
    char buf[BUF_SIZE];

    stdin_eof=0;
    FD_ZERO(&read_set);

    while(1){
        if(stdin_eof==0) FD_SET(fileno(fp), &read_set);
        FD_SET(sockfd, &read_set);
        max_fd=fmax(fileno(fp),sockfd)+1;
        
        if(select(max_fd, &read_set, NULL, NULL, NULL)<0) break;

        if(FD_ISSET(sockfd, &read_set)){
            if((n=read(sockfd, buf, BUF_SIZE))==0){   //FIN 받음
                if(stdin_eof==1) return;    //내가 이미 shutdown 한 상태에서 FIN을 받은 이 경우에만 최종 종료!
                else error_handling("모든 데이터를 보내기 전 서버로부터 FIN을 받음");
            }

            write(fileno(stdout),buf,n);    //futs 대신 write 사용!! 
        }

        if(FD_ISSET(fileno(fp),&read_set)){
            if((n=read(fileno(fp),buf,BUF_SIZE))==0){ //ctrl+D 입력
                stdin_eof=1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(sockfd, &read_set);
                continue;
            }

            write(sockfd, buf, n);
        }
    }
}

void error_handling(char *msg){
	perror(msg);
	fputc('\n',stderr);
	exit(1);
}