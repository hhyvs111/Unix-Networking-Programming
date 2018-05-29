#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
int main()
{

	int fd = mkfifo("num.pipe",0666);
	
	int numProc = 2;
	int baseNum = 0;
	printf("father id:%d\n",getpid());
	while(1)
	{
		if(numProc == 2)
			baseNum = 1;
		if(numProc == 1)
			baseNum = 0;
		if(fork() > 0)//在主进程里创建
		{
			if(--numProc == 0)
				break;
		}
		else
		{
			printf("child id:%d\n",getpid());

			int fd = open("num.pipe",O_RDWR);
			int num = baseNum;//之前创建进程的时候规定了，是奇数或者偶数
			while(1)
			{
				write(fd,&num ,4);
				num += 2;
				sleep(1);	
			}
			exit(0);//当子进程事情做完后退出,是退出的子进程而不是父进程
			//break;
		}
	}

	{
		int fd = open("num.pipe",O_RDONLY);
		int num = 0;
		while(1)
		{
			read(fd,&num,4);
			printf("%d\n",num);
		}
	}
	return 0;	
}
