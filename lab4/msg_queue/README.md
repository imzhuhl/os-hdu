#### 题目
利用 linux 的消息队列通信机制实现两个线程间的通信:
编写程序创建两个线程:sender 线程和 receive 线程，其中 sender 线程运行函数 sender()， 它创建一个消息队列，然后，循环等待用户通过终端输入一串字符，将这串字符通过消息队 列发送给 receiver 线程，直到用户输入“exit”为止;最后，它向 receiver 线程发送消息“end”， 并且等待 receiver 的应答，等到应答消息后，将接收到的应答信息显示在终端屏幕上，删除 相关消息队列，结束程序的运行。Receiver 线程运行 receive()，它通过消息队列接收来自 sender 的消息，将消息显示在终端屏幕上，直至收到内容为“end”的消息为止，此时，它 向 sender 发送一个应答消息“over”，结束程序的运行。使用无名信号量实现两个线程之间 的同步与互斥。

#### 使用
```shell
# please input this command in your terminal
gcc msg_queue.c -lpthread -o msg_queue
```

#### 说明
线程相关函数:

```c
pthread_create
pthread_join
pthread_exit
```

1. 写线程先 sleep 10 秒，期间测试消息队列的异步能力。
2. 可以进行队列容量测试，写线程的处去掉 `memset` 函数处注释，然后注释上一句 `scanf` 即可。
3. 添加了 `Ctrl c` 或 `∧ c` 程序终止信号的处理，2 的容量测试需要手动终止。