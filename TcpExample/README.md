#TCP服务器

## 0 环境配置

本机基于Ubuntu 16.04

需要安装LIBXML2库：
```
sudo apt-get install libxml2

sudo apt-get install libxml2-dev
```
使用makefile进行编译
```
make
```

服务端运行：
```
./Server 8000
```

客户端运行：
```
./Client 127.0.0.1
```
端口和服务器都可以改。

## 1 需求
使用TCP协议设计服务器，需要读取配置文件中的端口、认证用户名和密码进行客户端的认证，配置文件用XML文件实现。

### 1.1 命令支持
#### 1.1.1 LOGIN
客户输入用户名和密码进行登录，应该不用选择端口号。
#### 1.1.2 TIME
返回服务器的当前时间
#### 1.1.3 QUIT
退出程序

### 1.2 SESS功能
TIME和QUIT需要加SESS才能进行认证

## 2 设计
### 2.1 服务器
使用多进程TCP服务器，设计一个进程池或者一个客户端对应一个进程，实现多并发。

#### 2.1.1 SESS设计
定一个一个全局指针数组变量，用来存放对应客户端的SESS。SESS为随机数生成的随机字符串。

#### 2.1.2 共享内存和信号量
如果多个客户端（进程）同时访问XML文件会出错，所以要设计一个信号量实现互斥锁。Posix基于内存的信号量可以实现该功能，但是要将其放入共享内存

#### 2.1.3 功能
主要功能：
    （1）登录 
    读取XML文件，验证客户端发来的账户密码，如果匹配成功则生成SESS并发送给客户端；
    （2）时间 
    客户端发送TIME命令和SESS，验证本地存储的SESS，如果匹配则发送服务器时间给客户端；
    （3）退出
    客户端发送QUIT，对比SESS，匹配成功则返回信息给客户端并关闭子进程。
### 2.2 客户端
简单的C程序，输入命令。

#### 2.2.1 多进程
 - 父进程用来输入命令，处理命令后发送给服务器；
 - 子进程用来读服务器返回的值。

#### 2.2.2 SESS设计
使用共享内存保存SESS，子进程读取到SESS后存在***共享内存。父进程在处理TIME和QUIT时要读取共享内存里的SESS然后拼接成字符串发送给服务器

#### 2.2.3 基本验证
如果没有登录则不能输入TIME和QUIT命令，登录的依据为本地SESS是否为空。


## 3 主要难点

1 如何生成和保存SESS以及认证，使用随机数生成sess或者一些验证算法。

2 如何解析客户端发过来的字符串，通过正则表达式或者字符串分割

3 进程间通信，共享内存和信号量的使用

4 生成与解析XML文件

## 4 项目文件
1 tcp.h 所有头文件和相关宏定义

2 TcpServer.c 服务器

3 TcpClient.c 客户端

4 readXML.c 解析XML

5 produceXML.c  生成XML