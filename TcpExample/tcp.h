//网络编程主要要用到的头文件
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
#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <pthread.h>
#include<libxml/parser.h>
#include<libxml/xpath.h>
#include<libxml/tree.h>
#include <sys/shm.h> //共享内存
#include <semaphore.h>
#define MAXLINE 1024

#define LISTENQ 20

#define PERM S_IRUSR|S_IWUSR
/* include readn */

/*ssize_t						[> Read "n" bytes from a descriptor. <]*/
//readn(int fd, void *vptr, size_t n)
//{
	//size_t	nleft;
	//ssize_t	nread;
	//char	*ptr;

	//ptr = vptr;
	//nleft = n;
	//while (nleft > 0) {
		//if ( (nread = read(fd, ptr, nleft)) < 0) {
			//if (errno == EINTR)
				//nread = 0;		[> and call read() again <]
			//else
				//return(-1);
		//} else if (nread == 0)
			//break;				[> EOF <]

		//nleft -= nread;
		//ptr   += nread;
	//}
	//return(n - nleft);		[> return >= 0 <]
//}
//[> end readn <]

//ssize_t
//Readn(int fd, void *ptr, size_t nbytes)
//{
	//ssize_t		n;

	//if ( (n = readn(fd, ptr, nbytes)) < 0)
		//printf("readn error");
	//return(n);
//}
//[> include writen <]
//ssize_t						[> Write "n" bytes to a descriptor. <]
//writen(int fd, const void *vptr, size_t n)
//{
	//size_t		nleft;
	//ssize_t		nwritten;
	//const char	*ptr;

	//ptr = vptr;
	//nleft = n;
	//while (nleft > 0) {
		//if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			//if (nwritten < 0 && errno == EINTR)
				//nwritten = 0;		[> and call write() again <]
			//else
				//return(-1);			[> error <]
		//}

		//nleft -= nwritten;
		//ptr   += nwritten;
	//}
	//return(n);
//}
//[> end writen <]

//void
//Writen(int fd, void *ptr, size_t nbytes)
//{
	//if (writen(fd, ptr, nbytes) != nbytes)
		//printf("writen error");
/*}*/
