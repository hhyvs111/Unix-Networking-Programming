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
#define MAXLINE 1024



int main(int argc, char **argv)
{
	int sockfd,n;
	char send[MAXLINE];
	char recv[MAXLINE];
	struct sockaddr_in servaddr;

	if( argc !=2 )
		printf("please input ip address\n");

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		printf("sock err!\n");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13);

	inet_pton(AF_INET,argv[1] , &servaddr.sin_addr);
	if(connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr))  < 0)
		printf("connect error!\n");

   scanf("%s",send);
	/*scanf("%s",&(account->password));*/
	//写入
	int num = write(sockfd, send, MAXLINE);
	//printf("write size : %d\n",num);

	int num1 = read(sockfd,recv,MAXLINE);
	printf("read %d\n",num1);
	fputs(recv,stdout);
	return 0;
}
