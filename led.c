#include <linux/module.h>  //模块的头文件
#include <asm/io.h>   //io操作的头文件
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


void led_on(void)
{
    writel(readl(gpio_base+MSK_CNF)|1<<7,gpio_base+MSK_CNF); //取消对GPIO模式下引脚的屏蔽
    //为了只改变对应位，先把寄存器值读出来，再改变对应位写回去，这样就不会改变其他位置
    writel(readl(gpio_base+OE)|1<<7,gpio_base+MSK_OE);
    writel(readl(gpio_base+MSK_OUT)|1<<7,gpio_base+MSK_OUT);
    
}

void led_off(void)
{
    
}

int led_init(void)
{
    //printk("gpio out=%x",readl(GPIO3_OUT));//会报错，内核层不能直接访问硬件层地址
    unsigned char *out;
   // out=ioremap(GPIO3_OUT,4); //ioremap从物理地址GPIO3_OUT开始，映射4字节长度的空间
    gpio_base=ioremap(GPIO3,0XFF);//从物理地址GPIO3开始，映射0XFF字节长度的空间凹内核空间
    if(gpio_base==NULL)
    {
        printk("ioremap GPIO3_OUT error\n");
        return -1;   //返回负值表示失败
    }
    printk("gpio out=%x",readl(out));
    printk("led init ok\n");
    return 0;  //内核里返回值0表示成功，负值表示失败
}

void led_exit(void)
{
    iounmap(gpio_base);//卸载驱动时解除映射
    printk("led exit ok\n");
}

module_init(led_init);//内核模块加载入口 insmod led.ko 会触发调用 led_init
module_exit(led_exit);//声明led_exit是内核模块的卸载入口 rmmod led 会触发调用函数 led_exit
MODULE_LICENSE("GPL v2");//遵循的开源声明