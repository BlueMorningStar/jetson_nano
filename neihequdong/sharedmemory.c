//server_mm.c
#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<sys/shm.h>

int main(void)
{
    int shm = shmget((key_t)1006,4096,IPC_CREAT|IPC_EXCL|0666); //默认权限
    if(shm<0)
    {
        perror("shmget");
        return 2;
    }
    printf("%d\n",shm);
    //映射共享内存到进程数据段
    char* mem=(char*)shmat(shm,NULL,0);
    while(1)
    {
        //读取共享内存
        printf("server receive :%s\n",mem);
        sleep(1);
    }
    sleep(2);
    shmdt(mem); //分离，共享内存和进程
    shmctl(shm,IPC_RMID,NULL);//删除共享内存
    return 0;
}