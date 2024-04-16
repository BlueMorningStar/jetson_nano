#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#define LED_MA 500  //主设备号
#define LED_MI 0    //次设备号
#define LED_NUM 1   //有多少个设备

#define C_BUF_LEN 64
char c_buf[C_BUF_LEN]

//---偏移量
#define CNF     0X04  //配置寄存器（0：GPIO 1:SFIO）
#define OE      0X14  //输出使能寄存器(1:使能 0：关闭)
#define OUT     0X24  //输出寄存器（1：高电平  0：低电平）
#define MSK_CNF 0X84  //配置屏蔽寄存器（高位1：屏蔽 高位0：不屏蔽  低位1:GPIO模式  低位0:SFIO模式）
#define MSK_OE  0X94  //输出使能屏蔽寄存器（高位1：禁止写 低位1：使能）
#define MSK_OUT 0XA4  //输出屏蔽寄存器（高位1：禁止写   低位1：高电平）

#define LED_MAGIC 'L'       //幻数，一个驱动一个幻数，防止不同驱动命令错乱
#define LED_ON  _IOW(LED_MAGIC,0,int)
#define LED_OFF _IOW(LED_MAGIC,1,int)

struct resource *res1;
struct resource *res2;

unsigned char * gpio_base=NULL;
static int led_probe(struct platform_device *pdev)
{
    int devno=MKDEV(LED_MA,LED_MI);//合并主次设备号
    int ret;
    printk("match ok,plat_probe go\n");
    res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    /***********************************************
        platform_get_resource 从平台设备里获取资源
        pdev:指向平台设备的指针
        IORESOURCE_MEM:资源类型
    ************************************************/
    if(res1==NULL)
    {
        printk("platform_get_resource 1 fail\r\n");
        goto errRes1;
    }
    res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    if(res2==NULL)
    {
        printk("platform_get_resource 2 fail\r\n");
        goto errRes2;
    }
    ret = register_chrdev_region(devno,1,"scx_led");//注册字符设备，静态方式
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
    gpio_base=ioremap(res1->start,0XFF);//从物理地址GPIO3开始，映射0XFF字节长度的空间凹内核空间
    if(gpio_base==NULL)
    {
        printk("ioremap GPIO3_OUT error\n");
        goto err_unmap;
    }
    writel(readl(gpio_base+MSK_CNF)|1<<7,gpio_base+MSK_CNF); //取消对GPIO模式下引脚的屏蔽
    //为了只改变对应位，先把寄存器值读出来，再改变对应位写回去，这样就不会改变其他位置
    writel(readl(gpio_base+OE)|1<<7,gpio_base+MSK_OE);
    writel(readl(gpio_base+MSK_OUT)|1<<7,gpio_base+MSK_OUT);

    printk("plat_probe success\r\n");

    return 0;
    err_unmap:
         iounmap(gpio_base);
    err_cdev_del:
         cde_del(&cdev);
    err_register:
         unregister_chrdev_region(devno,1);
    errRes2:
    errRes1:
        return -1;
}

static int led_remove(struct platform_device *pdev)
{   
    unregister_chrdev_region(devno,1);
    cdev_del(&cdev);
    printk("led exit\r\n");
    printk("plat_remove go\r\n");
    return 0;
}

static const struct of_device_id of_plat_match[] = {//与驱动匹配的平台设备id列表
    {.compatible="yhai-led",},
    {}
}
MODULE_DEVICE_TABLE(of,of_plat_match);

static struct platform_driver led_driver = {
    .driver={
        .name = "yhai led driver",
        .owner = THIS_MODULE,
        .of_match_table = of_plat_match,
    },
    .probe = led_probe,
    .remove = led_probe,
};
module_platform_driver(led_driver); //声明平台设备驱动的入口
MODULE_LICENSE("GPL v2");//遵循的开源声明