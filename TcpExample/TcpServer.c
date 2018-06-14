#include "tcp.h"   //网络编程所需头文件

/* 作者：谭文波
 * 时间：2018年6月4日 10:03:28
 * 程序：TCP服务器
 *
 */

//解析XML配置文件，将用户名和密码作为参数传入函数。返回值1为登录成功，0为失败
int readXML(char*, char*);

//回收子进程，防止出现僵尸进程
void sig_child(int);

//处理客户端请求函数
void function(int);

void function_time(int);

void function_login(char*,int);

void function_quit(int);

//生成指定长度随机字符串:SESS
char *gen_random_sess(int);

//全局变量

//客户端标识符
int g_nowClient ;
//存放客户端的SESS
char *g_sess[1024];
//共享内存key，用来存放信号量，使得所有进程都可以访问
int g_shmid;
//信号量，用于互斥读取XML文件
sem_t *g_mysem;

int main(int argc,  char **argv)
{
    int listenfd,  connfd;
    pid_t childpid;
    if (argc != 2)
    {
        printf("please input the port\n");
        exit(1);
    }

    int port  = atoi(argv[1]);    //端口号
    g_nowClient = 0; //初始化客户端数目为0
    struct sockaddr_in servaddr;
    //又忘记加括号导致if出错
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket error!\n");
        exit(1);
    }
    bzero(&servaddr,  sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);   //端口为输入的值
    signal(SIGCHLD,  sig_child);    //绑定信号函数
    if (bind(listenfd,  (struct sockaddr *)&servaddr,  sizeof(servaddr)) < 0 )
    {
        printf("bind error!\n");
        exit(1);
    }
    if (listen(listenfd,  LISTENQ) == 0)
    {
        printf("start listening\n");
    }
    else
    {
        printf("listen error!\n");
        exit(1);
    }
    //创建共享内存区
    if ((g_shmid=shmget(IPC_PRIVATE,  1024,  PERM))==-1)
    {
          fprintf(stderr, "create share memory error:%s\n\a", strerror(errno));
          exit(1);
    }
    //将信号连接到共享内存，返回值0为成功，-1为失败
    if ((g_mysem = shmat(g_shmid,  0,  0)) == NULL )
    {
        printf("mysem connect shm error\n");
        exit(1);
    }
     //基于内存的信号量，第二位参数1代表进程间共享
    if (sem_init(g_mysem,  1,  1))
    {
        printf("init sem error!\n");
        exit(1);
    }

    //主循环
    for (; ;)
    {
        connfd = accept(listenfd,  (struct sockaddr *)NULL,  NULL);

        g_nowClient++;
        printf("welcome the %dth client!\n",  g_nowClient);

        //子进程
        if ((childpid = fork()) == 0)
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
    while((pid = waitpid(-1,  &stat,  WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return ;
}

//产生指定长度的随机字符串
char* gen_random_sess(int length)
{
    if(length < 1 )
        return NULL;
    int flag,  i;
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
    char *delim = "?";     //分隔符
    int n;
    char *command, *content;   //命令和内容
    char *saveptr;
    //一直读，直到读到QUIT
    char *msg;
    if((msg = malloc(MAXLINE)) == NULL)  // sess=
            printf("malloc error /msg\n");
    memset(msg, 0, MAXLINE);
    for(; ;)
    {
        sleep(1);
        //字符格式： command?content
        if((n = recv(sockfd, recvbuff, MAXLINE, 0)) <= 0)
        {
            printf("read error!\n");
            close(sockfd);
            exit(1);
        }
        recvbuff[n] = 0;
        printf("recvlen:%d, recv: %s\n", n, recvbuff);
        command = strtok_r(recvbuff, delim, &saveptr);
        content = strtok_r(NULL, delim, &saveptr);
        //printf("conmand :%s ,  content :%s\n", command, content);
        if(!strncmp(command, "LOGIN",sizeof("LOGIN")))
        {
            function_login(content,sockfd);
        }
        //之所以放在一起是因为二者都需要判断SESS
        else if(!strncmp(command, "TIME",sizeof("TIME")) || !strncmp(command, "QUIT",sizeof("QUIT")))
        {
            char *delim1 = "=";
            char *subContent;
            char *getSess;
            char *saveptr;
            subContent = strtok_r(content, delim1, &saveptr);   //将SESS= XXXX 分割
            getSess = strtok_r(NULL, delim1, &saveptr);
            printf("the subContent:%s , the getSess:%s\n", subContent, getSess);
            if(strncmp(subContent, "SESS", sizeof("SESS")) != 0)
            {
                printf("is not correct content\n");
                continue;
            }
            else
            {
                printf("getsess:%s ,  g_sess:%s\n", getSess, g_sess[g_nowClient]);
                //父子进程虽然共享该全局变量，但是子进程的是父进程的拷贝，即使父进程进来了多个客户端，g_nowClinet还是一开始fork之后的值
                if(strncmp(getSess, g_sess[g_nowClient], sizeof(g_sess[g_nowClient])) != 0)
                {
                    printf("is error sess!");
                    /*msg = "is error sess!";*/
                    memset(msg, 0, MAXLINE);
                    char *error_msg = "is error sess!";
                    strncpy(msg, error_msg, strlen(error_msg));
                    int ret = send(sockfd, msg, strlen(msg), 0);
                    if(ret < 0)
                        exit(1);
                    else if (ret > 0)
                        printf("send %d bit\n", ret);
                    continue;
                }
                else
                {
                    //发送服务器时间
                    if(!strcmp(command, "TIME"))
                    {
                        function_time(sockfd);

                    }
                    //退出子进程
                    else if(!strcmp(command, "QUIT"))
                    {
                        function_quit(sockfd);
                    }
                }
            }
        }
    }
    free(msg);
    return;
}


void function_login(char * content,int sockfd)
{
    char *msg;
    if((msg = malloc(MAXLINE)) == NULL)  // sess=
            printf("malloc error /msg\n");
    memset(msg, 0, MAXLINE);
    //解析XML里的数据
    //需要将Login后面的字符串分解
    char *user_name, *pass_name, *user, *pass;

    //需要使用线程安全的strtok_r可重入版本。将结构保存在saveptr中，而不是静态变量里
    char *saveptr;
    //printf("the content :%s\n", content);
    char *delim1 = "&";
    user_name = strtok_r(content, delim1, &saveptr);
    //printf("user_name : %s\n");
    pass_name = strtok_r(NULL, delim1, &saveptr);
    //printf(" pass_name : %s\n", pass_name);
    char *delim2 = "=";
    strtok_r(user_name, delim2, &saveptr);
    user = strtok_r(NULL, delim2, &saveptr);

    strtok_r(pass_name, delim2, &saveptr);
    pass = strtok_r(NULL, delim2, &saveptr);

    printf("user : %s,  password:%s \n",  user, pass);
    //这个时候user和pass就是用户输入的内容
    //需要通过信号量 共享内存互斥访问XML文件
    if ((g_mysem = shmat(g_shmid, 0, 0)) == NULL)
    {
        printf("shmat error sem_wait!\n");
        exit(1);
    }
    if (sem_wait(g_mysem))
    {
        printf("sem_wait error!\n");
        exit(1);
    }

    if(readXML(user, pass) == 1)
    {
        g_sess[g_nowClient] = gen_random_sess(10);   //生成长度为10的字符串sess
        printf("the sess is : %s", g_sess[g_nowClient]);
        memset(msg, 0, MAXLINE);
        strncpy(msg, "SESS=", sizeof("SESS=") - 1);   //使用安全的字符串复制函数
        strncat(msg, g_sess[g_nowClient],sizeof(g_sess[g_nowClient]));
        printf("the msg will send to Client : %s\n", msg);
        int ret = send(sockfd, msg, strlen(msg), 0);
        free(msg);
        if(ret < 0)
            exit(1);
        else if (ret > 0)
            printf("send %d bit\n", ret);
    }
    else    //返回错误
    {
        memset(msg, 0, MAXLINE);
        char *error_msg = "ERROR=error username or password!";
        strncpy(msg, error_msg,sizeof(error_msg));
        int ret = send(sockfd, msg, strlen(msg), 0);
        free(msg);
        if(ret < 0)
            exit(1);
        else if (ret > 0)
            printf("send %d bit\n", ret);
    }
    if ((g_mysem = shmat(g_shmid, 0, 0)) == NULL)
    {
        printf("shmat error sem_wait!\n");
        exit(1);
    }
    if (sem_post(g_mysem))
    {
        printf("sem_post error!\n");
        exit(1);
    }
}

void function_time(int sockfd)
{

    char *msg;
    if((msg = malloc(MAXLINE)) == NULL)  // sess=
            printf("malloc error /msg\n");
    memset(msg, 0, MAXLINE);
    time_t   timep;
    timep = time(NULL);
    memset(msg, 0, MAXLINE);
    strncpy(msg, "TIME=",sizeof("TIME="));
    strncat(msg, ctime(&timep),strlen(ctime(&timep)));
    int ret = send(sockfd, msg, strlen(msg), 0);
    free(msg);
    if(ret < 0)
        exit(1);
    else if (ret > 0)
        printf("send %d bit\n", ret);
}

void function_quit(int sockfd)
{
    char *msg;
    if((msg = malloc(MAXLINE)) == NULL)  // sess=
            printf("malloc error /msg\n");
    memset(msg, 0, MAXLINE);
    strncpy(msg, "QUIT=",sizeof("QUIT="));
    strncat(msg, g_sess[g_nowClient],strlen(g_sess[g_nowClient]));
    int ret = send(sockfd, msg, strlen(msg), 0);
    free(msg);
    if(ret < 0)
        exit(1);
    else if (ret > 0)
        printf("send %d bit\n", ret);
    printf("good bye %dth client!\n", g_nowClient);
    exit(1);    //子进程退出
}
