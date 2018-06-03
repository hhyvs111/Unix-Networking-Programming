#include "tcp.h"

char *g_sess;  //g_sess全局变量，子进程也需要使用

int main(int argc, char **argv)
{
	int sockfd,n;
    pid_t childpid;
	char recv[MAXLINE],send[MAXLINE];
	struct sockaddr_in servaddr;


    void read_msg(int);
    void sig_child(int);

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

    //子进程读消息

    if((childpid = fork()) == 0 )
    {
        close(sockfd);
        read_msg(sockfd);
        exit(0);
    }
    //主进程进入命令行模式
    char *command;
    printf("just support 3 command:\n LOGIN TIME QUIT \n");
    for(; ;)
    {
        printf("input command!");
        scanf("%s",command);

        if(strcmp(command,"LOGIN") == 0)
        {
            if(g_sess != NULL)
            {
                printf("you have already logged in\n");
            }
            else
            {
                printf("username:");
                char *user,*pass;
                scanf("%s",user);
                printf("password:");
                scanf("%s",pass);

                char *result = malloc(strlen(command) + strlen(user) + strlen(pass) + 17 ); //?NAME=&PASSWORD=
                if(result == NULL) exit(1);
                strcpy(result,command);

                //将命令、账户密码拼接成一串字符发到服务器
                strcat(result,"?NAME=");
                strcat(result,user);
                strcat(result,"&PASSWORD=");
                strcat(result,pass);

                Writen(sockfd,result,strlen(result));
                free(result);
            }
        }
        else if(strcmp(command,"TIME") == 0)
        {
            if(g_sess == NULL)
            {
                printf("please LOGIN!\n");
            }
            else
            {
                char *result = malloc(strlen(command) + strlen(g_sess) + 7);     //?sess=
                if(result == NULL) exit(1);
                strcpy(result,command);
                strcpy(result,"?SESS=");
                strcpy(result,g_sess);
                Writen(sockfd,result,strlen(result));
                free(result);
            }
        }
        else if(strcmp(command,"QUIT") == 0)
        {
            if(g_sess == NULL)
            {
                printf("please LOGIN!\n");
            }
            else
            {
                char *result = malloc(strlen(command) + strlen(g_sess) + 7);  //?QUIT=
                if(result == NULL) exit(1);

                strcpy(result,command);
                strcpy(result,"?SESS=");
                strcpy(result,g_sess);

                Writen(sockfd,result,strlen(result));
                free(result);
                exit(0);  //退出程序，子进程会变孤儿进程啊
            }
        }
        else
        {
            printf("invalid command!\n");
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
         exit(0); //主进程也关闭
     }
}
//子进程读消息并回显在屏幕上
void read_msg(int sockfd)
{
    char reply[MAXLINE];

    char *command;
    char *delim = "=";
    int n;
    for(; ;)
    {
        if(( n == Readn(sockfd,reply,MAXLINE)) == 0)
            printf("%s\n",reply);

        //解析信息，将sess存入g_sess
        command = strtok(reply,delim);
        if(strcmp(command,"SESS") == 0)
        {
            while(g_sess = strtok(NULL,delim));
        }
        else if(strcmp(command,"QUIT") == 0)
        {
            exit(0);  //子进程退出后通知主进程关闭
        }

    }
    return ;
}
