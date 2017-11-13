#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>
static int all_task_init(void)
{
    struct task_struct *p;
    for_each_process(p) {
        printk(KERN_ALERT"%s\t%d\t%d\t%d\n", p->comm, p->pid, p->state, p->prio);
    }
    return 0;
}
static void all_task_exit(void)
{
    printk(KERN_ALERT"all_task module exit\n");
}

module_init(all_task_init);
module_exit(all_task_exit);
MODULE_LICENSE("GPL");
