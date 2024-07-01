//driver.c 驱动：设备有I/O操作时唤醒。
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>  
#include <asm/io.h>  
#include <asm/uaccess.h> 
#include <linux/sched.h>
#include <linux/poll.h>

static wait_queue_head_t queue;  //定义等待队列
struct fasync_struct *async_queue; //定义异步通知

static ssize_t write_dev(struct file *flie, const char _user *buf,size_t count, loff_t *f_ops)
{
    
}