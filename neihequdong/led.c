#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>//字符设备头文件

#define LED_MA 500  //主设备号
#define LED_MI 0    //次设备号
#define LED_NUM 1   //有多少个设备

#define LED_MAGIC 'L'       //幻数，一个驱动一个幻数，防止不同驱动命令错乱
#define LED_ON  _IOW(LED_MAGIC,0,int)
#define LED_OFF _IOW(LED_MAGIC,1,int)

struct cdev led_cdev;
int led_open(struct inode* inode,struct file* file)
{
    printk("i can open\r\n");
    return 0;
}

int  led_release(struct inode* inode,struct file* file)
{
    printk("led release\r\n");
    return 0;
}

long led_unlocked_ioctl(struct file*, unsigned int cmd,unsigned long args)
{
    switch(cmd)
    {
        case LED_ON:
            printk("led on \r\n");
        break;
        case LED_OFF:
            printk("led off \r\n");
        break;
        
    }
}

struct file_operations led_fops=
{  
    .owner=THIS_MODULE,
    .open=led_open,
    .release=led.release,
    .unlocked_ioctl=led_unlocked_ioctl,
};
static int led_init(void)
{
    int devno=MKDEV(LED_MA,LED_MI);//合并主次设备号
    int ret;
    register_chrdev_region(devno,1,"scx_led");//注册字符设备，静态方式
    /*  1.dev_id:分配的起始设备号
        2.DEVICE_NUM: 请求的连续设备编号的总数（不能天大，避免别的主设备号冲突）
        3.DEVICE_NAME:是应当连接到这个编号范围的设备的名字
        4.alloc_chrdev_region  可进行动态分配
    */
    if(ret<0) 
    {
        printk("register_chrdev_region fail\r\n");
        return -1;
    }
    cdev_init(&cdev,&led_fops);//字符设备初始化
    ret=cdev_add(&cdev,devno,1);
        if(ret<0) 
    {
        printk("cdev_add fail\r\n");
        return -1;
    }
    printk("led init\r\n");
    return 0;
}

static void led_exit(void)
{
    unregister_chrdev_region(devno,1);
    cdev_del(&cdev);
    printk("led exit\r\n");
}

module_init(led_init);
modlue_exit(led_exit);
MODULE_AUTHOR("scx");
MODULE_LISENCE("GPL");