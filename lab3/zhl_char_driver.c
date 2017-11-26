#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static ssize_t my_write(struct file *file, const char __user *buff, size_t count, loff_t *offp);
static loff_t my_llseek(struct file *filp, loff_t offset, int whence);

/* file struct*/
static struct file_operations zhl_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .llseek = my_llseek,
    .unlocked_ioctl = my_ioctl,
    .read = my_read,
    .write = my_write,
};

struct mem_dev
{
    struct cdev *cdevp;
    unsigned char mem[512];
};

struct cdev cdev;
struct mem_dev *zhl_devp;
// 设备号 和 数量
static const int dev_n = 255;
static const int dev_ct = 1;

/* init and exit module*/
static int char_drive_init(void)
{
    int reg_ret = register_chrdev_region(MKDEV(dev_n, 0), dev_ct, "zhl_char_driver");
    if (reg_ret != 0) {
        return 0;
    }
    // init cdev
    cdev_init(&cdev, &zhl_fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &zhl_fops;
    // add cdev
    cdev_add(&cdev, MKDEV(dev_n, 0), dev_ct);

    zhl_devp = kmalloc(sizeof(struct mem_dev), GFP_KERNEL);
    zhl_devp->cdevp = &cdev;
    memset(zhl_devp->mem, '\0', sizeof(char) * 512);

    printk("z_char_driver start\n");
}

static void char_drive_exit(void)
{
    cdev_del(&cdev);
    kfree(zhl_devp);
    unregister_chrdev_region(MKDEV(dev_n, 0), dev_ct);
    printk("z_char_driver exit\n");
}

module_init(char_drive_init);
module_exit(char_drive_exit);


static int my_open(struct inode *inode, struct file *filp)
{
    filp->private_data = zhl_devp;
    printk("z_char_driver open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{
    printk("z_char_driver release\n");
    return 0;
}

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    printk("z_char_driver ioctl");
    return 0;
}

// 修改偏移量
static loff_t my_llseek(struct file *filp, loff_t offset, int whence)
{
    unsigned long p = offset;
    // SEEK_SET only
    if (whence != 0) {
        return -1;
    }
    if (p < 0 || p > 512) {
        return -1;
    }
    filp->f_pos = offset;
    return offset;
}

static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    // offset
    unsigned long p = *offp;
    unsigned int size = count;
    int ret = 0;
    struct mem_dev *dev = filp->private_data;

    if (size + p > 512) {
        return -1;
    }
    ret = copy_to_user(buff, (void *)(dev->mem + p), size);
    if (ret) {
        return -1;
    }

    *offp += size;
    printk("read %d bytes from offset addr %d\n", size, p);
    return size;

}

static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    unsigned long p = *offp;
    unsigned int size = count;
    int ret = 0;
    struct mem_dev *dev = filp->private_data;

    if (size + p > 512) {
        return -1;
    }

    ret = copy_from_user(dev->mem + p, buff, size);
    if (ret) {
        return -1;
    }

    *offp += size;
    printk("written %d bytes from offset addr %d\n", size, p);
    return size;
}