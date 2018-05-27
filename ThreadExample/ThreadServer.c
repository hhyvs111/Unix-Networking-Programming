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
#include <pthread.h>
#define MAXLINE 1024
/**
 *实现TCP线程服务器
 */
//线程运行函数
static void *run(void *);

int main(int argc, char **argv)
{

    int listenfd, *iptr;//iptr is a point , point to connfd, so we don't need connfd
	ssize_t n, len;
    pthread_t tid;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);// create socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);

	if(bind(listenfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
		printf("bind error\n");
	listen(listenfd,10);
	printf("begin listening!\n");
	for(; ;)
	{
        len = sizeof(servaddr);
        iptr = malloc(sizeof(int));
	    *iptr = accept(listenfd, (struct sockaddr *)NULL,&len);
        printf("new connection\n");
        pthread_create(&tid,NULL,&run,iptr);
    }
}


static void *run(void *arg)
{
    int connfd, n, val;
    val = fcntl(connfd, F_GETFL, 0);
    fcntl(connfd, F_SETFL, val | O_NONBLOCK);
    //set the socket is no block
    char recv[MAXLINE];
    connfd = *((int *) arg);

    free(arg);
    pthread_detach(pthread_self());
    while((n = read(connfd,recv, MAXLINE)) > 0)
    {
        recv[n] = 0;
        fputs(recv, stdout);
    }
    if(n < 0)
    {
        printf("read error!");
    }
    close(connfd);
    return(NULL);
}


