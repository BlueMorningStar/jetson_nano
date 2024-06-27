#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/ktthread.h>


struct task_struct *task_id = NULL;

int new_kthread(void *arg)
{
    int n = 0;
    while(1)
    {
        printk("%s:%d\n",__func__,n++);
        ssleep(3);//ssleep是按秒睡眠，msleep是按毫秒睡眠
        if(kthread_should_stop())  //接收停止信号
        {
            break;
        }
    }
    return 0;
}

static int __init base_init(void)
{
    printk("%s\n",__func__);
    task_id = kthread_create(new_kthread,NULL,"new a kthread");  //创建新内核线程

    if(IS_ERR(task_id))
    {
        printk("kthread_create err\n");
        return -1;
    }

    wake_up_process(task_id); //唤醒内核线程（通过id指定）
    /* 对比
    kthread_run(new_kthread,NULL,"new and run a ktrhead");//创建并运行内核线程
    (是kthread_create+wake_up_process)
    kernel_thread(kernel_init,NULL, CLONE_FS|CLONE_SIGHAND);  //1号进程(内核启动方式)
    kthread_create:
    创建的内核线程有干净的上下文环境，适合于驱动模块或用户空间的程序创建内核线程使用，不会把某些内核信息暴露给用户程序
    kernel_thread: 创建的进程可以是init或其他内核线程，而kthread_create:创建的进程的父进程被指定为kthreadd
    */
   return 0;
}


static void __exit base_exit(void)
{
    printk("%s:\n",__func__);
    kthread_stop(task_id);
    return -1;
}

module_init(base_init);
module_exit(base_exit);