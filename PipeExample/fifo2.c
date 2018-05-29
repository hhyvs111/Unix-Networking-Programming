#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

//实现读fifo.c所输出的管道文件
//用于进程间通信的
int main()
{

	//mkfifo("2.pipe",0666); //0666可读写权限
	
	int fd = open("2.pipe",O_RDONLY);//返回一个文件描述符，RDONLY只读
	int i = 0;
	while(1)
	{
		read(fd,&i,4);
		printf("%d\n",i);
		sleep(1);
	}
	close(fd);

	return 0;
}
