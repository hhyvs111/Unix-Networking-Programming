#include "tcp.h"

char *g_sess;  //g_sess全局变量，子进程也需要使用
int shmid;

int main(int argc, char **argv)
{
	int sockfd,n;
    pid_t childpid;
	char recv[MAXLINE];
	struct sockaddr_in servaddr;

    void read_msg(int);
    void sig_child(int);

    if((shmid=shmget(IPC_PRIVATE,1024,PERM))==-1)
    {
        fprintf(stderr,"create share memory error:%s\n\a",strerror(errno));
        exit(1);
    }
    if( argc !=2 )
    {
	    printf("please input ip address\n");
        return 0;
    }

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("sock err!\n");
    }
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);        //暂时默认8000端口

	inet_pton(AF_INET,argv[1] , &servaddr.sin_addr);
	if(connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr))  < 0)
    {
        printf("connect error!\n");
        return 0;
    }
    else
        printf("connect success!\n");


    signal(SIGCHLD,sig_child);
    /*signal(SIGUSR1,sig_main);   //父进程退出的时候发送SIGUSER1*/
    //子进程读消息

    if((childpid = fork()) == 0 )
    {
        /*close(sockfd);*/
        read_msg(sockfd);
        close(sockfd);
        exit(1);
    }
    else
    {
        //主进程进入命令行模式
        char command[MAXLINE];
        printf("just support 3 command:\nLOGIN TIME QUIT \n");
        char *result;
        if((result= malloc(MAXLINE)) == NULL)
            printf("malloc error result\n");
        memset(result,0,MAXLINE);
        for(; ;)
        {
            printf("input command:\n");
            scanf("%s",command);

            printf("the command is %s\n",command);

            if(strcmp(command,"LOGIN") == 0)
            {
                g_sess = shmat(shmid, 0, 0);  // 读共享内存里的数据
                /*printf("g_sess:%s\n",g_sess);*/
                if(strlen(g_sess) != 0 )
                {
                    printf("you have already logged in\n");
                }
                else
                {
                    printf("please input username:");
                    char user[30],pass[30];
                    scanf("%s",user);
                    printf("please input password:");
                    scanf("%s",pass);

                    if(result == NULL) exit(1);
                    memset(result,0,MAXLINE);
                    strcpy(result,command);
                    //将命令、账户密码拼接成一串字符发到服务器
                    strcat(result,"?NAME=");
                    strcat(result,user);
                    strcat(result,"&PASSWORD=");
                    strcat(result,pass);
                    /*printf("the msg will send : %s\n",result);*/
                    int ret = send(sockfd,result,strlen(result),0);
                    if(ret == -1)
                    {
                        printf("send error:LOGIN\n");
                        exit(1);
                    }
                }
            }
            else if(strcmp(command,"TIME") == 0)
            {
                g_sess = shmat(shmid, 0, 0);  // 读共享内存里的数据
                if(strlen(g_sess) == 0 )  //这里要考虑进程间共享变量，使用共享内存
                {
                    printf("please LOGIN!\n");
                }
                else
                {
                    memset(result,0,MAXLINE);
                    strcpy(result,command);
                    strcat(result,"?SESS=");
                    strcat(result,g_sess);
                    /*printf("the result is :%s\n",result);*/
                     int ret = send(sockfd,result,strlen(result),0);
                    if(ret == -1)
                        exit(1);
                }
            }
            else if(strcmp(command,"QUIT") == 0)
            {
                g_sess = shmat(shmid, 0, 0);  // 读共享内存里的数据
                if(strlen(g_sess) == 0)
                {
                    printf("please LOGIN!\n");
                }
                else
                {
                    memset(result,0,MAXLINE);
                    strcpy(result,command);
                    strcat(result,"?SESS=");
                    strcat(result,g_sess);
                    int ret = send(sockfd,result,strlen(result),0);
                    if(ret == -1)
                        exit(1);

                    /*Writen(sockfd,result,strlen(result));*/
                    free(result);
                    /*exit(1);  //退出程序，子进程会变孤儿进程*/
                }
            }
            else
            {
                printf("invalid command!\n");
            }
            sleep(1);
        }
    }

    return 0;
}



void sig_child(int signo)
{

     pid_t pid;
     int stat;
     while((pid = waitpid(-1, &stat,WNOHANG)) > 0)
     {
         printf("goodbye!\n");

         //主进程也关闭
         exit(1);
     }
}

//子进程读消息并回显在屏幕上
void read_msg(int sockfd)
{
    /*printf("begin read msg!\n");*/
    char *reply;

    char *command,*tmp_sess;
    char *delim = "=";
    int n;
    if((reply = malloc(MAXLINE)) == NULL)
    {
        printf("malloc error :reply\n");
        exit(1);
    }

    for(; ;)
    {
        memset(reply,0,MAXLINE);
        if(( n = recv(sockfd,reply,MAXLINE,0)) <= 0)    //这里写了个BUG，测试半天没发现，多写了个=，变成 n == recv了，怪不得一直出错
        {
            printf("socket error\n");
            exit(1);
        }
        printf("%s\n",reply);
        //解析信息，将sess存入g_sess
        command = strtok(reply,delim);
        /*printf("command:%s\n",command);*/
        if(strcmp(command,"SESS") == 0)
        {
            printf("Login success!\n");
            tmp_sess = strtok(NULL,delim);
            g_sess = shmat(shmid,0,0);
            memset(g_sess,0,1024);
            strcpy(g_sess,tmp_sess);
            printf("the sess:%s\n",g_sess);
        }
        else if(strcmp(command,"QUIT") == 0)
        {
            close(sockfd);
            shmctl(shmid,IPC_RMID,0);  //删除共享内存区
            exit(1);  //子进程退出后通知主进程关闭*/
        }
        else  // 如果发送的都不是以上信息
        {
            continue;
        }
    }
    return ;
}
