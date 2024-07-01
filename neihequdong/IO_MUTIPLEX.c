//app.c 应用: 阻塞等待事件发生， 有事件，则轮询找出发生事件的设备
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>


int main()
{
    int fd;
    int ret;
    fd_set rfds;
    char buff[5];

    fd = open("/dev/uart",O_RDONLY|O_NONBLOCK); //只读，非阻塞
    if(fd < 0)
    {
        printf("Device open failure\n");
    }
    while(1)
    {
        FD_ZERO(&rfds); //清空读集合
        FD_SET(fd,&rfds); //添加设备到读即可
        int nready = select(fd+1,&rfds,NULL,NULL,NULL); 
        /*
        阻塞等待某设备有事件发生 ->交由内核去遍历解除阻塞
        I/O多路复用:select ->sys_poll ->驱动里.poll函数
        操作流程:
        1. 拷贝fds到内核空间
        2. 遍历fds, 把当前进程添加到，每个fd设备的等待队列中(fd:是设备的文件句柄)
        3. 当某个fd设备有I/O数据时， 其驱动会唤醒该设备等待队列上睡眠的进程(见驱动的.write函数)
        4. 系统调用sys_poll去轮询，调用驱动.poll通过返回值知道，是否有I/O事件发生，有则解除阻塞，返回应用层
        */
       if(nready<=0)
       {
        printf("select err or timeout\n");
        return -1;
       }
       if(FD_ISSET(fd,&rfds))  //数据可获得
       {
        printf("poll monitor: can be read\n");
        ret = read(fd,buff,sizeof(buff)-1); //读数据，无数据则马上返回
        if(ret<0)
        {
            printf("read err\n");
            return -1;
        }
        printf("read buf is %s\n",buff);
       }
       printf("select go\n");
    }
}