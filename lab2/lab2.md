
## 问题
(1) 设计一个模块，要求列出系统中所有内核线程的程序名、PID 号、进程状态及 进程优先级。

(2) 设计一个带参数的模块，其参数为某个进程的 PID 号，该模块的功能是列出该 进程的家族信息，包括父进程、兄弟进程和子进程的程序名、PID 号。

## 相关源码

```c
#ifndef __kernel_pid_t
typedef int     __kernel_pid_t;
#endif
typedef __kernel_pid_t      pid_t;

/* Task command name length: */
#define TASK_COMM_LEN   16;

struct task_struct {
    pid_t               pid;
    volatile long       state;
    int                 prio;
    /*
     * executable name, excluding path.
     *
     * - normally initialized setup_new_exec()
     * - access it with [gs]et_task_comm()
     * - lock it with task_lock()
     */
    char                comm[TASK_COMM_LEN];

    /* Recipient of SIGCHLD, wait4() reports: */
    struct task_struct __rcu    *parent;

    /*
     * Children/sibling form the list of natural children:
     */
    struct list_head        children;
    struct list_head        sibling;
};

struct list_head {
    struct list_head *next, *prev;
};

```
