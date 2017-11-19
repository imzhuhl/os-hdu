#include <linux/kernel.c>
#include <linux/module.c>
#include <linux/interrupt.c>

static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static int my_ioctl(struct inode *inode, struct file *filp);
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static ssize_t my_write(struct file *file, const char __user *buff, size_t count, loff_t *offp);

/* file struct*/
static struct file_operations zhl_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .ioctl = my_ioctl,
    .read = my_read,
    .write = my_write,
};
/* cdev struct */
struct mem_dev
{
    struct cdev *cdevp;
    unsigned char mem[512];
};

struct cdev cdev;
struct mem_dev *zhl_devp;

/* init and exit module*/
static int __init char_drive_init(void)
{
    
    int dev_n = 255;
    int reg_ret = register_chrdev_region(MKDEV(dev_n, 0), 1, "zhl_char_driver");
    if (reg_ret != 0) {
        return 0;
    }
    // init cdev
    cdev_init(&cdev, &zhl_fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &zhl_fops;

    // add cdev
    cdev_add(&cdev, MKDEV(dev_n, 0), 1);
    zhl_devp = kmalloc(sizeof(struct mem_dev), GFP_KERNEL);
    memset(zhl_devp, 0, sizeof(struct mem_dev));

}

static void __exit char_drive_exit(void)
{
    cdev_del(&cdev);
    kfree(zhl_devp);
    unregister_chrdev_region(MKDEV(255, 0), 1);
}

module_init(char_drive_init);
module_exit(char_drive_exit);

static int my_open(struct inode *inode, struct file *filp)
{
    // some code
    printk("mem char drive open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{
    // some code
    printk("mem char drive release\n");
    return 0;
}

static int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    // ....
}

static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
}

static ssize_t my_write(struct file *file, const char __user *buff, size_t count, loff_t *offp)
{
}
