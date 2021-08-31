// 다중접속 에코 서버
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *msg);
void read_childproc(int sig);

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;

    pid_t pid;
    struct sigaction action;
    socklen_t addr_size;
    int str_len, state;
    char buf[BUF_SIZE];
    
    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    //시그널 등록
    action.sa_handler=read_childproc; //시그널 발생 시 호출할 함수
    sigemptyset(&action.sa_mask);   
    action.sa_flags=0;
    state=sigaction(SIGCHLD, &action, 0);   //(시그널 정보, 시그널 핸들러 정보)

    //서버소켓 주소설정
    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    //서버소켓 할당(bind), 접속대기(listen)
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock,5)==-1)
        error_handling("listen() error");

    while(1){
        addr_size=sizeof(clnt_addr);
        //클라이언트 접속 허용(accept)
        clnt_sock=accept(serv_sock, (struct sockaddr*) &clnt_addr,&addr_size);
        if(clnt_sock==-1)
            continue;
        else   
            puts("새로운 클라이언트 접속 완료...");
        
        //클라이언트 접속마다 자식 생성해 프로세스 생성
        pid=fork();
        if(pid==-1){
            close(clnt_sock);
            continue;
        }
        if(pid==0){ //자식 프로세스 -> 에코 기능 구현
            close(serv_sock);
            while((str_len=read(clnt_sock,buf,BUF_SIZE))!=0){
                write(clnt_sock,buf,str_len);
            }

            close(clnt_sock);
            puts("클라이언트 연결 해제 완료...");
            return 0;
        }
        else //부모 프로세스
            close(clnt_sock);
    }
    close(serv_sock);
    return 0;  
}

//시그널 발생(자식 종료)시 핸들러 작동
void read_childproc(int sig){
    pid_t pid;
    int status;
    pid=waitpid(-1,&status,WNOHANG);
    printf("removed proc id: %d\n",pid);
}

void error_handling(char *msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}