#include "tcp.h"   //网络编程所需头文件

/* 作者：谭文波
 * 时间：2018年6月3日 10:03:28
 * 程序：TCP服务器
 *
 */

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;

    void sig_child(int), function(int);  // 函数声明

    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(argv[1]);   //端口为输入的值

    signal(SIGCHLD, sig_chld);    //绑定信号函数
    if(bind(listenfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
       › printf("bind error\n");
    if(listen(listenfd,LISTENQ) == 0)
        printf("start listening\n");
    else
        printf("listen error!\n");
    //主循环
    for(; ;)
    {
        connfd = accept(listenfd, (struct sockaddr *)NULL,NULL);

        //子进程
        if( (childpid = fork()) == 0 )
        {
            close(listenfd);
            function(connfd);   //功能函数，Login time等实现
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    printf("server shutdown!\n");
    return 0;
}

//防止出现僵尸进程
void sig_child(int signo)
{
    pid_t pid;
    int stat;
    while((pid = waitpid(-1 , &stat, WNOHANG)) > 0)
        printf("child %d terminated\n",pid);
    return ;
}

//读取客户端的发来的信息并处理，跳转到相应的函数
void function(int sockfd)
{
    if(sockfd < 0)
    {
         return -1;
    }
    char recvbuff[MAXLINE];     //MAXLINE为1024
    char *msg;
    char *delim = "?";     //分隔符

    char *command, *content;   //命令和内容
    //一直读，直到读到QUIT
    for(; ;)
    {
        //字符格式： command?content
        if((n = Readn(sockfd,recvbuff,MAXLINE)) == 0)
        {
            printf("read error!\n");
        }

        command = strtok(recvbuff,delim);
        while(content = strtok(NULL,delim))

        printf("conmand :%s , content :%s \n",command,content);

        if(strcmp(command,"LOGIN"))
        {
            //解析XML里的数据

            //需要将Login后面的字符串分解
            char *user,*pass;

            char *delim1 = "&";
            user = strtok(content,delim1);
            while(pass = strtok(NULL,delim1));

            char *delim2 = "=";
            strtok(user,delim2);
            while(user = strtok(NULL,delim2));
            strtok(pass,delim2);
            while(pass = strtok(NULL,delim2));

            //这个时候user和pass就是用户输入的内容
            int ret;
           if((ret = readXML(&user,&pass)) == 1)  //登录成功，返回sess
           {
                msg = "Login success!";
                Writen(sockfd,msg,strlen(msg));
           }
           else    //返回错误
           {
                msg = "Login faild!";
                Writen(sockfd,msg,strlen(msg));
           }
        }
    }

}
