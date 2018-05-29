#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<fcntl.h>
#include<signal.h>

int numChild = 0;	//子进程数全局变量
void childProcEnd(int s)
{
	int status;
	
	if(s == SIGCHLD)
	{
		printf("delete child%d\n",numChild);
		numChild++;
		wait(&status);
	}		
	if(numChild == 2)
	{
		printf("all child delete\n");
		exit(0);
	}
}
int isSushu(int num)
{
	int tp = num;
	for(int i = 2;i < tp;i++)
	{
		if(num%i == 0) return 0;
	}
	return 1;
}

int main()
{

//	int fd = mkfifo("num.pipe",0666);
	
	int numProc = 2;
	int fds[2];
	pipe(fds);
	int a,b;
	signal(SIGCHLD,childProcEnd);
	printf("father id:%d\n",getpid());
	while(1)
	{
		if(numProc == 2)
		{
			a = 2;
			b = 100;
		}
		if(numProc == 1)
		{
			a = 101;
			b = 200;
		}
		if(fork() > 0)//在主进程里创建
		{
			if(--numProc == 0)
				break;
		}
		else
		{
			close(fds[0]);
			for(int i = a;i <= b;i++)
			{
				if(isSushu(i) == 1)
				{
					write(fds[1] ,&i, 4);
					printf("%d\n",i);
				}
			}
			exit(0);//当子进程事情做完后退出,是退出的子进程而不是父进程
			//子进程退出，等待主进程的回收，以防僵死进程
			//break;
		}
	}
	
	{
		close(fds[1]); //把写关闭
		int wfd = open("result.txt",O_RDWR | O_CREAT);
		while(1)
		{
			int num = 0;
		  int ret = read(fds[0],&num, 4);
			if(ret == 4)
				write(wfd,&num,4);
			printf("father %d\n",num);
		}
	}

	return 0;	
}
