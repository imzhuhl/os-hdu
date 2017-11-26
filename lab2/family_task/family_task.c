#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/moduleparam.h>
MODULE_LICENSE("GPL");

static int pid;
module_param(pid, int, 0644);

static int family_task_init(void)
{
    struct task_struct *own, *parent, *ptr = NULL;
    struct list_head *head, *list_ptr = NULL;
    // 获得 sibling 在 task_struct 中的相对偏移量
    unsigned long sibling_offset = &((struct task_struct *)0)->sibling;
    
    for_each_process(own) {
        if(own->pid == pid) {
            printk("This process: comm=%s\t pid=%d\n", own->comm, pid);
            // his parent
            parent = own->parent;
            printk("his parent: comm=%s\t pid=%d\n", parent->comm, parent->pid);
            // his children
            head = &own->children;
            list_ptr = head->next;
            while(list_ptr != head) {
                ptr = (struct task_struct *)((char *)list_ptr - sibling_offset);
                printk("children: comm=%s\t pid=%d\n", ptr->comm, ptr->pid);
                list_ptr = list_ptr->next;
            }
            // his sibling
            head = &own->parent->children;
            list_ptr = head->next;
            while(list_ptr != head) {
                ptr = (struct task_struct *)((char *)list_ptr - sibling_offset);
                if (ptr->pid != pid) {
                    printk("sibling: comm=%s\t pid=%d\n", ptr->comm, ptr->pid);
                }
                list_ptr = list_ptr->next;
            }

            break;
        }
    }
    return 0;
}

static void family_task_exit(void)
{
    printk("familly_task_mod exit\n");
}

module_init(family_task_init);
module_exit(family_task_exit);