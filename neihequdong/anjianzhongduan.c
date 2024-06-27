#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

struct key_dev{ //定义按键的类
struct device *pdev;
const char *name;
unsigned int *gpio_num;
unsigned int irq_num;
};

enum of_gpio_flags flags;

irqreturn_t key_interrupt(int irqno,void* devid)
{
    pr_info("d0 interrupt irqno=%d\n",irqno);
    return IRQ_HANDLED;
}

int keys_probe(struct platform_device *pdev)
{
    int ret;
    struct key_dev *key;
    key = devm_kzalloc(&pdev->dev, sizeof(*key),GFP_KERNEL);
    if(key==NULL)
    {
        pr_err("devm_kzalloc fail\n");
        goto devm_kzalloc_err;
    }
    key->dev = &pdev->dev;
    key->name = "scx_key";
    key->gpio_num = of_get_named_gpio_flags(pdev->dev.of_node,"key_gpio",0,&flags); //获取设备树节点的属性
    if(gpio_is_valid(key->gpio_num))
    {
        pr_err("gpio is not valid\n");
        goto err_of_get_named_gpio_flags;
    }
    key->irq_num = gpio_to_irq(key->gpio_num);//根据IO编号获取其中断号
    //这里是软件编号只是为了区分，不是硬件芯片手册上的编号
   ret = devm_request_irq(&pdev->dev,
                     key->irq_num, //中断号（用于区分不同中断）
                     key_interrupt, //中断处理函数
                     IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,//触发方式
                     key_name, //中断名称，在cat/proc/interrupts中看到
                     key);//共享中断时才用（共享一中断线，用于标识中断处理程序，中断标志需设为IRQF_SHARED）

                     
    pr_notice("keys_probe ok \n");
    return 0;



    err_of_get_named_gpio_flags:
        
    devm_kzalloc_err:
        return - ENMEM;
}





int keys_remove(struct platform_device *pdev)
{
    printk("keys_remove ok\n");
    return 0;
}

/*平台设备模版部分 */
static const struct of_device_id of_key_match={
    {
        .compatible = "yhai_key_irq",   //匹配设备树的ID
    },
    {},
};
MODULE_DEVICE_TABLE(of,of_key_match);

struct platform_driver keys_drv = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "yhai_key_irq_driver",
        .of_match_table = of_key_match,
    },
    .probe = keys_probe,
    .remove = keys_remove,
};

modlue_platform_driver(keys_drv);
MODULE_LICENSE("GPL");