#include	"unp.h"

void
str_cli(FILE *fp, int sockfd)		//这里是fp，应该是从文件读内容到sock
{
	int			maxfdp1;
	fd_set		rset;		//检查可读性的描述符集
	char		sendline[MAXLINE], recvline[MAXLINE];

	FD_ZERO(&rset);		//初始化该集合
	for ( ; ; ) {
		//The function fileno() examines the argument stream and returns its integer descriptor.

		FD_SET(fileno(fp), &rset);	//fileno函数把标准I/O文件指针转换为对应的描述符
		FD_SET(sockfd, &rset);		//FD_SET打开两位
		/*
		FD_ZERO() clears a set.  FD_SET()  and  FD_CLR()  respec‐
       tively  add and remove a given file descriptor from a set.  FD_ISSET() tests to see if a file descriptor is part of the set; this is useful after select() returns.
	*/
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
			if (Readline(sockfd, recvline, MAXLINE) == 0)
				err_quit("str_cli: server terminated prematurely");
			Fputs(recvline, stdout);
		}

		if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
			if (Fgets(sendline, MAXLINE, fp) == NULL)
				return;		/* all done */
			Writen(sockfd, sendline, strlen(sendline));
		}
	}
}
