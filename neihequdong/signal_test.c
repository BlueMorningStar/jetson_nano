/*******应用层**********/

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void sigint_handle(int signo)
{
    switch(signo)
    {
        case SIGINT:
        printf("get a signal ---SIGINT\n");
        break;
    }
    return;
}

int main()
{
    signal(SIGINT,sigint_handle); //设置信号的处理函数-->发现ctrl+c不再终止进程了（改写了默认的行为）
    //改用ctrl+\发生QUIT信号来终止程序
    while(1)
    {
        sleep(1);
    }
}