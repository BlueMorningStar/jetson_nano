//client_mm.c
#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<sys/shm.h>
int main()
{
	//申请共享内存(key值，大小，权限等标志)
	int shm=shmget((key_t)1006,4096,IPC_CREAT);
	if(shm<0){
		perror("shmget");
		return 2;
	}

	//关联共享内存(映射共享内存到进程数据段)
	char* mem=shmat(shm,NULL,0);
	int i=0;
	while(1)
	{
		//写共享内存
		mem[i]='a'+i;
		i++;
		mem[i]='\0';
		sleep(1);
	}

	shmdt(mem);//取消关联（分离 共享内存和进程）
	return 0;
}