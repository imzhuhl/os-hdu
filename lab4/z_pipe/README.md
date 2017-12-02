#### 题目
实现一个管道通信程序:
由父进程创建一个管道，然后再创建 3 个子进程，并由这三个子进程利用管道与父进程 之间进行通信:子进程发送信息，父进程等三个子进程全部发完消息后再接收信息。通信的 具体内容可根据自己的需要随意设计，要求能试验阻塞型读写过程中的各种情况，测试管道 的默认大小，并且要求利用 Posix 信号量机制实现进程间对管道的互斥访问。运行程序，观 察各种情况下，进程实际读写的字节数以及进程阻塞唤醒的情况。

#### 使用
```shell
# please input this command in your terminal
gcc z_pipe.c -lpthread -o z_pipe
```

#### 说明