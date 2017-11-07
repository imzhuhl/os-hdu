## 题目

添加一个系统调用，实现对指定进程的 nice 值的修改或读取功能，并返回进程最 新的 nice 值及优先级 prio。建议调用原型为:
```c
int mysetnice(pid_t pid, int flag, int nicevalue, void __user * prio, void __user * nice);
```

## 过程

#### 修改系统调用号
```shell
vim ./arch/x86/entry/syscalls/syscall_64.tbl

333 64      mysetnice       sys_mysetnice
```

#### 申明系统调用服务例程原型

```c
vim ./include/linux/syscalls.h

asmlinkage long sys_mysetnice(pid_t pid, int flag, int nicevalue, void __user *prio, void __user *nice);
```

#### 实现函数
进入 kernel 目录下的 sys.c文件
```c
SYSCALL_DEFINE5(mysetnice, pid_t, pid, int, flag, int, nicevalue, void __user *, prio, void __user *, nice)
{
    if(pid < 0 || (flag != 1 && flag != 0)){
        return EFAULT;
    }
    struct task_struct *p;
    void * my_nice;
    void * my_prio;
    for_each_process(p) {
        if(p->pid == pid){
            if(flag == 1){
                set_user_nice(p, nicevalue);
            }
            *(int*)my_nice = task_nice(p);
            *(int*)my_prio = task_prio(p);
            // 用户空间与内核空间数据拷贝
            copy_to_user(prio, my_prio, 8);
            copy_to_user(nice, my_nice, 8);
            return 0;
        }
    }
    return EFAULT;
}
```
以上完成,就可以编译内核了.

#### 测试代码
```c
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#define __NR_mysyscall 333

int main()
{
    int pid = 0;
    int flag = 0;
    int nicevalue = 0;
    int prio = 0;
    int nice = 0;
    int result;

    printf("please input: pid, flag, nicevalue\n");
    scanf("%d %d %d", &pid, &flag, &nicevalue);
    result = syscall(__NR_mysyscall, pid, flag, nicevalue, (void *)&prio, (void *)&nice);
    if (result == 0)
    {
        printf("pid:%d, flag:%d, nicevalue:%d\n", pid, flag, nicevalue);
        return 0;
    }
    printf("some wrong, maybe pid is not exist\n");
    return 0;
}
```

#### 解释
代码解释(linux 源码):
```c
// in linux/sched.h
#define for_each_process(p) \
        for (p = &init_task ; (p = next_task(p)) != &init_task ; )

// in linux/sched.h
#define next_task(p)    list_entry((p)->tasks.next, struct task_struct, tasks)

// in linux/list.h
/*
 * list_entry - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
        container_of(ptr, type, member)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

/**
 * 关于获取prio 和 nice
 */
// in linux/sched/prio.h
#define MAX_NICE	19
#define MIN_NICE	-20
#define NICE_WIDTH	(MAX_NICE - MIN_NICE + 1)
#define MAX_USER_RT_PRIO	100
#define MAX_RT_PRIO		MAX_USER_RT_PRIO
#define DEFAULT_PRIO		(MAX_RT_PRIO + NICE_WIDTH / 2)
#define NICE_TO_PRIO(nice)	((nice) + DEFAULT_PRIO)
#define PRIO_TO_NICE(prio)	((prio) - DEFAULT_PRIO)

// in linux/sched.h
static inline int task_nice(const struct task_struct *p)
{
	return PRIO_TO_NICE((p)->static_prio);
}
// in linux/sched/core.c
int task_prio(const struct task_struct *p)
{
	return p->prio - MAX_RT_PRIO;
}

/**
 * 关于修改nice
 */
void set_user_nice(struct task_struct *p, long nice)
{
    // 省略....
    p->static_prio = NICE_TO_PRIO(nice);
    p->prio = effective_prio(p);
    // 省略....
}

// in linux/sched/core.c
static int effective_prio(struct task_struct *p)
{
	p->normal_prio = normal_prio(p);
	/*
	 * If we are RT tasks or we were boosted to RT priority,
	 * keep the priority unchanged. Otherwise, update priority
	 * to the normal priority:
	 */
	if (!rt_prio(p->prio))
		return p->normal_prio;
	return p->prio;
}
static inline int normal_prio(struct task_struct *p)
{
	int prio;

	if (task_has_dl_policy(p))
		prio = MAX_DL_PRIO-1;
	else if (task_has_rt_policy(p))
		prio = MAX_RT_PRIO-1 - p->rt_priority;
	else
		prio = __normal_prio(p);
	return prio;
}

static inline int task_has_dl_policy(struct task_struct *p)
{
    return dl_policy(p->policy);
}
static inline int dl_policy(int policy)
{
    return policy == SCHED_DEADLINE;
}

static inline int task_has_rt_policy(struct task_struct *p)
{
    return rt_policy(p->policy);
}
static inline int rt_policy(int policy)
{
    return policy == SCHED_FIFO || policy == SCHED_RR;
}

static inline int __normal_prio(struct task_struct *p)
{
    return p->static_prio;
}

static inline int rt_prio(int prio)
{
    if (unlikely(prio < MAX_RT_PRIO))
        return 1;
    return 0;
}
```



