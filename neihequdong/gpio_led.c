#include <linux/init.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
int led_gpiod_probe(struct platform_device* pdev)
{
    struct gpio_desc *gpiod = NULL;
    gpiod = gpiod_get(&pdev->dev, NULL, GPIOD_OUT_HIGH);
    IF(gpiod == NULL){
        printk("gpiod_get error\r\n");
        return -ENODEV;
    }
    gpiod_direction_output(gpiod,1);
    platform_set_drvdata(pdev,gpiod);
    return 0;
}

int led_gpiod_remove(sturct platform_device* pdev)
{
    struct gpio_desc *gpiod = platform_get_drvdata(pdev);
    gpiod_set_value(gpiod,0);
    gpiod_put(gpiod); //回收gpio口的gpio_desc资源
    printk("led_gpiod_remove ok\n");
}

struct of_device_id ids[]={
    {.compatible = "yhai,led_gpiod"},
    {},
};

struct platform_driver led_gpiod_drv = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "led_gpiod_drv",
        .of_match_table = ids,
    },
    .probe = led_gpiod_probe,
    .remove = led_gpiod_remove,
};




module_platform_driver(led_gpiod_drv);
MODULE_LICENSE("GPL");