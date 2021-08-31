#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
void error_handling(char *message);

int main(){
    int fd;    //파일 디스크립터 번호
    char buf[]="Hello Socket Programming!!\n";
    
    //open 함수-> 디스크립터 번호 반환.
    fd=open("test.txt",O_CREAT|O_WRONLY|O_TRUNC);
    if(fd==-1)
        error_handling("open() error!");
    printf("file descriptor: %d\n",fd);

    //파일에 쓰기. write(디스크립터 번호, 데이터의 주소값, 데이터의 바이트수) -> 전달한 바이트수 반환
    if(write(fd,buf,sizeof(buf))==-1)
        error_handling("write() error!");
    
    //파일(또는 소켓)은 항상 닫아주기!!
    close(fd);
    return 0;
}

void error_handling(char *message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}