#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<signal.h>

//实现循环写入数字到管道
int fd = 0;
//信号函数
void endProc(int s)
{
	close(fd);
	unlink("2.pipe"); //删除管道文件和rm命令作用一样，都是删除文件
	exit(0);
}

//用于进程间通信的
int main()
{
	signal(SIGINT, endProc);

	mkfifo("2.pipe",0666); //0666可读写权限
	
    fd = open("2.pipe",O_RDWR);//返回一个文件描述符,如果用只写的方式就会独占用这个文件
	int i = 1;
	while(1)
	{
		printf("data %d\n",i);
		write(fd,&i,4);
		i++;
		sleep(1);
	}
	return 0;
}
