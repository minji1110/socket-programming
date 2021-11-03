#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 10
void error_handling(char *msg);

int main(int argc, char const *argv[])
{
    int listen_fd, conn_fd;
    struct sockaddr_in serv_addr, clnt_addr;   //<arpa/inet.h>
    socklen_t clnt_len;
    fd_set read_set, temp_set;
    int max_fd, max_conn, nready;
    int client[FD_SETSIZE];
    char buf[BUF_SIZE];

    if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0)
        error_handling("socket() error");
    
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));
    serv_addr.sin_family=AF_INET;

    if(bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
        error_handling("bind() error");

    if(listen(listen_fd,10)<0)
        error_handling("listen() error");
    
    max_fd=listen_fd;   //read_set에서 감시하는 fd의 최대 번호
    max_conn=-1;        //연결 설정된 소켓의 최대 번호
    for(int i=0;i<FD_SETSIZE;i++){
        client[i]=-1;
    }

    FD_ZERO(&read_set);
    FD_SET(listen_fd, &read_set);

    while(1){
        temp_set=read_set;
        if((nready=select(max_fd+1, &temp_set, NULL, NULL, NULL))<0) break;

        //readable한 fd가 리스닝 소켓
        if(FD_ISSET(listen_fd,&temp_set)){
            printf("select return : listening socket, nready=%d\n",nready);
            clnt_len=sizeof(clnt_addr);
            conn_fd=accept(listen_fd,(struct sockaddr*) &clnt_addr, &clnt_len);
            
            for(int i=0;i<=FD_SETSIZE;i++){
                if(i==FD_SETSIZE) error_handling("too many clients!!");
                
                if(client[i]<0) {
                    client[i]=conn_fd;
                    if(max_conn<i) max_conn=i;
                    break;
                }
            }
            
            FD_SET(conn_fd,&read_set);
            if(conn_fd>max_fd) max_fd = conn_fd;
            printf("connect! fd= %d\n",conn_fd);

            //listening 소켓이 끝이면 connect 소켓들은 볼 필요 없음, 다시 select 대기 상태로
            if(--nready<=0) continue;
         
        }

        int readable_fd,n;
        //readable 한 fd가 client 소켓 중 존재
        for(int i=0;i<=max_conn;i++){
            if((readable_fd=client[i])<0) continue;

            if(FD_ISSET(readable_fd,&temp_set)){
                if((n=read(readable_fd, buf, BUF_SIZE))==0){
                    FD_CLR(readable_fd,&read_set);
                    close(readable_fd);
                    client[i]=-1;
                    printf("close! fd=%d\n",readable_fd);
                }
                else{
                    write(readable_fd, buf, n);
                    fputc('\n',stdout);
                    sleep(3);
                }

                //더이상 readable한 소켓 없는경우 뒷부분은 안봐도 됨, 다시 select 대기 상태로
                if(--nready<=0) break; 
            }
        }
    }

    close(listen_fd);
    return 0;
}

void error_handling(char *msg){
    perror(msg);
	fputc('\n',stderr);
	exit(1);
}