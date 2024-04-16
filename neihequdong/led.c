#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>//字符设备头文件
#include <asm/uaccess.h> //for read write

#define LED_MA 500  //主设备号
#define LED_MI 0    //次设备号
#define LED_NUM 1   //有多少个设备

#define C_BUF_LEN 64
char c_buf[C_BUF_LEN]

#define LED_MAGIC 'L'       //幻数，一个驱动一个幻数，防止不同驱动命令错乱
#define LED_ON  _IOW(LED_MAGIC,0,int)
#define LED_OFF _IOW(LED_MAGIC,1,int)

#define GPIO3_OUT 0X6000D224 //控制GPIO3_PJ_7  硬件层地址，内核层不可直接访问
#define GPIO3     0X6000D200

//---偏移量
#define CNF     0X04  //配置寄存器（0：GPIO 1:SFIO）
#define OE      0X14  //输出使能寄存器(1:使能 0：关闭)
#define OUT     0X24  //输出寄存器（1：高电平  0：低电平）
#define MSK_CNF 0X84  //配置屏蔽寄存器（高位1：屏蔽 高位0：不屏蔽  低位1:GPIO模式  低位0:SFIO模式）
#define MSK_OE  0X94  //输出使能屏蔽寄存器（高位1：禁止写 低位1：使能）
#define MSK_OUT 0XA4  //输出屏蔽寄存器（高位1：禁止写   低位1：高电平）

unsigned char * gpio_base=NULL;

 void show_reg(void)  //查看相关寄存器的内容
 {
     printk("cnf =%x\n",readl(gpio_base+CNF));
     printk("oe =%x\n",readl(gpio_base+OE));
     printk("out =%x\n",readl(gpio_base+OUT));
     printk("mask cnf =%x\n",readl(gpio_base+MSK_CNF));
     printk("mask oe =%x\n",readl(gpio_base+MSK_OE));
     printk("mask out =%x\n",readl(gpio_base+MSK_OUT));
 }


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
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .unlocked_ioctl = led_unlocked_ioctl,
    .write = led_write,
    .read = led_read,
};
static int led_init(void)
{
    int devno=MKDEV(LED_MA,LED_MI);//合并主次设备号
    int ret;
    gpio_base=ioremap(GPIO3,0XFF);//从物理地址GPIO3开始，映射0XFF字节长度的空间凹内核空间
    if(gpio_base==NULL)
    {
        printk("ioremap GPIO3_OUT error\n");
        goto err_unmap;
    }
    ret = register_chrdev_region(devno,1,"scx_led");//注册字符设备，静态方式
    /*  1.dev_id:分配的起始设备号
        2.DEVICE_NUM: 请求的连续设备编号的总数（不能天大，避免别的主设备号冲突）
        3.DEVICE_NAME:是应当连接到这个编号范围的设备的名字
        4.alloc_chrdev_region  可进行动态分配
    */
    if(ret<0) 
    {
        printk("register_chrdev_region fail\r\n");
        goto err_register;
    }
    cdev_init(&cdev,&led_fops);//字符设备初始化
    ret=cdev_add(&cdev,devno,1);
    if(ret<0) 
    {
        printk("cdev_add fail\r\n");
        goto err_cdev_del;
    }
    writel(readl(gpio_base+MSK_CNF)|1<<7,gpio_base+MSK_CNF); //取消对GPIO模式下引脚的屏蔽
    //为了只改变对应位，先把寄存器值读出来，再改变对应位写回去，这样就不会改变其他位置
    writel(readl(gpio_base+OE)|1<<7,gpio_base+MSK_OE);
    writel(readl(gpio_base+MSK_OUT)|1<<7,gpio_base+MSK_OUT);
    printk("led init\r\n");
    return 0;

    err_cdev_del:
        cde_del(&cdev);

    err_register:
        unregister_chrdev_region(devno,1);
    err_unmap:
        iounmap(gpio_base);
    return -1;
}

static void led_exit(void)
{
    unregister_chrdev_region(devno,1);
    cdev_del(&cdev);
    printk("led exit\r\n");
}

//正数：写入成功的字节数 0：无成功写入的字节数 -1：错误码
static int led_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
    int ret;
    if(count>C_BUF_LEN)
    {
        printk("buf length overflow\r\n");
        return -ENOMEM;
    }
    if(count<0)
    {
        printk("length must be optisum\r\n");
        return -EINVAL; //Invalid arguments
    }
    ret=copy_from_user(c_buf,buf,count);//把应用层的数据拷贝到内核层 内核层buf,用户层buf,buf长度
    if(ret)//0表示成功，正数表示拷贝失败的长度
    {
        printk("copy_from_user fail \r\n");
        return -EFALT;     //错误码在Errn0-base.h里面定义
    }
    return ret; 
}

ssize_t led_read (struct file *file, char __user *buf, size_t count, loff_t *fop)
{
    int ret;
    if(count>C_BUF_LEN-1) count=C_BUF_LEN-1;
    if(count<0)
    {
        return -EINVAL;
    }
    ret=copy_to_user(buf,c_buf,count);  //用户层buf,内核层buf,buf长度
    if(ret)
    {
        printk("copy_to_user fail \r\n");
        return -EFAULT;
    }
    return ret;
}

module_init(led_init);
modlue_exit(led_exit);
MODULE_AUTHOR("scx");
MODULE_LISENCE("GPL");