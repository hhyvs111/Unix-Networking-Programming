#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define MAXLINE 1024

typedef struct 
{
	char username[30];
	char password[30];
}Account;

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1 , &stat, WNOHANG)) > 0)
		printf("child %d terminated\n",pid);
	return ;
}


int check(int sockfd)
{
	if(sockfd < 0)
	{
		return -1;
	}
	int n;
	int pos = 0;
	Account *account = (Account*)malloc(sizeof(Account));
	int needRead = sizeof(Account);
	char *buff = (char*)malloc(needRead);

	n = read(sockfd, buff, MAXLINE);
	printf("size :%d\n ",n);
	if(n < 0)
	{
		printf("read faild!\n");
	}
	memcpy(account,buff,needRead);
	printf("%s %s\n",account->username, account->password);
	if(!strcmp(account->username,"admin") && !strcmp(account->password, "123"))
		{
			free(account);
			free(buff);
			return 1;
		}

	else
	{
		free(account);
		free(buff);
	}
		return 0;				
}
int main(int argc, char **argv)
{
	int listenfd, connfd, nready, maxfdp1;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);
	signal(SIGCHLD, sig_chld);
		
	if(bind(listenfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
		printf("bind error\n");
	listen(listenfd,10);
	printf("begin listening!\n");
	for(; ;)
	{
		connfd = accept(listenfd, (struct sockaddr *)NULL,NULL);
		if( (childpid = fork()) == 0)
		{
			int result = 0; 	//0为失败，1为成功
			close(listenfd);
			result = check(connfd);
			if(result)
			{
				printf("Login success!\n");
				write(connfd, "success!\n",20);
			}

			else
			{
				write(connfd,"faild!",10);
				printf("Login faild!\n");
			}
			close(connfd);
			exit(0);
		}
		close(connfd);		
	}
}
