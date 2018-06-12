#include "tcp.h"   //网络编程所需头文件

/* 作者：谭文波
 * 时间：2018年6月4日 10:03:28
 * 程序：TCP服务器
 *
 */

int readXML(char*,char*);  //函数声明

int g_nowClient ;    //客户端ID
char *g_sess[1024];   //暂且支持1024个客户端
int shmid;    //共享内存，实现互斥访问XML文件
sem_t *mysem;       //Posix信号量

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;

    void sig_child(int), function(int);  // 函数声明
    int port  = atoi(argv[1]);
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);   //端口为输入的值
    signal(SIGCHLD, sig_child);    //绑定信号函数
    if(bind(listenfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
        printf("bind error\n");
    if(listen(listenfd,LISTENQ) == 0)
        printf("start listening\n");
    else
        printf("listen error!\n");

    g_nowClient = 0;
    if((shmid=shmget(IPC_PRIVATE,1024,PERM))==-1)
    {
          fprintf(stderr,"create share memory error:%s\n\a",strerror(errno));
          exit(1);
    }
    mysem = shmat(shmid, 0 ,0);      //将信号连接到共享内存
    sem_init(mysem,1 ,1);        //基于内存的信号量，第二位参数1代表进程间共享
    //主循环
    for(; ;)
    {
        connfd = accept(listenfd, (struct sockaddr *)NULL,NULL);

        g_nowClient++;
        printf("welcome the %dth client!\n",g_nowClient);

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

//产生指定长度的随机字符串
char* genRandomString(int length)
{
    int flag, i;
    char* string;
    srand((unsigned) time(NULL ));
    if ((string = (char*)malloc(length)) == NULL )
    {
        printf("Malloc failed!flag:14\n");
        return NULL ;
    }

    for (i = 0; i < length - 1; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
            case 0:
                string[i] = 'A' + rand() % 26;
                break;
            case 1:
                string[i] = 'a' + rand() % 26;
                break;
            case 2:
                string[i] = '0' + rand() % 10;
                break;
            default:
                string[i] = 'x';
                break;
        }
    }
    string[length - 1] = '\0';
    return string;
}

//读取客户端的发来的信息并处理，跳转到相应的函数
void function(int sockfd)
{
    if(sockfd < 0)
    {
         return ;
    }
    char recvbuff[MAXLINE];     //MAXLINE为1024
    char *msg;
    char *delim = "?";     //分隔符
    int n;
    char *command, *content;   //命令和内容
    //一直读，直到读到QUIT
    if((msg = malloc(MAXLINE)) == NULL)  // sess=
            printf("malloc error /msg\n");
    memset(msg,0,MAXLINE);
    for(; ;)
    {
        sleep(1);
        //字符格式： command?content
        if((n = recv(sockfd,recvbuff,MAXLINE,0)) <= 0)
        {
            printf("read error!\n");
            close(sockfd);
            exit(1);
        }
        recvbuff[n] = 0;
        printf("recvlen:%d ,recv: %s\n",n,recvbuff);
        command = strtok(recvbuff,delim);
        content = strtok(NULL,delim);
        //printf("conmand :%s , content :%s\n",command,content);
        if(!strcmp(command,"LOGIN"))
        {
            //解析XML里的数据
            //需要将Login后面的字符串分解
            char *user_name,*pass_name,*user,*pass;
            //printf("the content :%s\n",content);
            char *delim1 = "&";
            user_name = strtok(content,delim1);
            //printf("user_name : %s\n");
            pass_name = strtok(NULL,delim1);
            //printf(" pass_name : %s\n",pass_name);
            char *delim2 = "=";
            strtok(user_name,delim2);
            user = strtok(NULL,delim2);

            strtok(pass_name,delim2);
            pass = strtok(NULL,delim2);

            printf("user : %s, password:%s \n", user,pass);
            //这个时候user和pass就是用户输入的内容
            //需要通过信号量 共享内存互斥访问XML文件
            mysem = shmat(shmid,0,0);
            sem_wait(mysem);
            if(readXML(user,pass) == 1)
            {
                g_sess[g_nowClient] = genRandomString(10);   //生成长度为10的字符串sess
                printf("the sess is : %s",g_sess[g_nowClient]);
                memset(msg,0,MAXLINE);
                strcpy(msg,"SESS=");
                strcat(msg,g_sess[g_nowClient]);
                printf("the msg will send to Client : %s\n",msg);
                int ret = send(sockfd,msg,strlen(msg),0);
                if(ret < 0)
                    exit(1);
                else if (ret > 0)
                    printf("send %d bit\n",ret);
            }
            else    //返回错误
            {
                memset(msg,0,MAXLINE);
                strcpy(msg,"ERROR=error username or password!");
                int ret = send(sockfd,msg,strlen(msg),0);
                if(ret < 0)
                    exit(1);
                else if (ret > 0)
                    printf("send %d bit\n",ret);
            }
            mysem = shmat(shmid,0,0);
            sem_post(mysem);
        }
        else if(!strcmp(command,"TIME") || !strcmp(command,"QUIT"))
        {
            char *delim1 = "=";
            char *subContent;
            char *getSess;
            subContent = strtok(content,delim1);   //将SESS= XXXX 分割
            getSess = strtok(NULL,delim1);
            printf("the subContent:%s ,the getSess:%s\n",subContent,getSess);
            if(strcmp(subContent,"SESS") != 0)
            {
                printf("is not correct content\n");
                continue;
            }
            else
            {
                printf("getsess:%s , g_sess:%s\n",getSess,g_sess[g_nowClient]);
                //父子进程虽然共享该全局变量，但是子进程的是父进程的拷贝，即使父进程进来了多个客户端，g_nowClinet还是一开始fork之后的值
                if(strcmp(getSess,g_sess[g_nowClient]) != 0)
                {
                    printf("is error sess!");
                    /*msg = "is error sess!";*/
                    memset(msg,0,MAXLINE);
                    strcpy(msg,"is error sess!");
                    int ret = send(sockfd,msg,strlen(msg),0);
                    if(ret < 0)
                        exit(1);
                    else if (ret > 0)
                        printf("send %d bit\n",ret);
                    continue;
                }
                else
                {
                    //发送服务器时间
                    if(!strcmp(command,"TIME"))
                    {

                        time_t   timep;
                        timep = time(NULL);
                        memset(msg,0,MAXLINE);
                        strcpy(msg,"TIME=");
                        strcat(msg,ctime(&timep));
                        int ret = send(sockfd,msg,strlen(msg),0);
                        if(ret < 0)
                            exit(1);
                        else if (ret > 0)
                            printf("send %d bit\n",ret);
                    }
                    //退出子进程
                    else if(!strcmp(command,"QUIT"))
                    {
                        memset(msg,0,MAXLINE);
                        strcpy(msg,"QUIT=");
                        strcat(msg,g_sess[g_nowClient]);
                        int ret = send(sockfd,msg,strlen(msg),0);
                        if(ret < 0)
                            exit(1);
                        else if (ret > 0)
                            printf("send %d bit\n",ret);
                        printf("good bye %dth client!\n",g_nowClient);
                        exit(1);    //子进程退出
                    }
                }
            }
        }
    }
    return;
}
