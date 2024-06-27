#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
/**********进程就是APP**********/

int count; //用全局变量，让线程间共享数据

pthread_mutex_t lock; //定义锁

void *pth_work(void *arg)
{
  while (1) {
    pthread_mutex_lock(&lock); //上锁( 不能获得时，会阻塞睡眠)
                              //trylock  尝试请求锁，锁不可用时，直接return，不会线程卡住
    //sem_wait(&sem); //上锁，不能获得时，会阻塞睡眠                              
    count++;
    printf("pthread id=%d count=%d\n", pthread_self() , count); //pthread_self 获得线程自己的ID
    pthread_mutex_unlock(&lock); //解锁
    usleep(2000000);
    //sem_post(&sem);//解锁
  }
  return NULL;
}
void *pth_work2(void *arg)
{
  while (1) {
    pthread_mutex_lock(&lock);
    count=0;
    printf("pthread id=%d count=%d\n", pthread_self() , count); 
    pthread_mutex_unlock(&lock); 
    usleep(5000000);
  }
  return NULL;
}


int main(void)
{
  pthread_t id;  
  pthread_t id2;
  pthread_mutex_init(&lock, NULL); //构造锁
  //sem_init(&sem,0,1); //构建二元信号量
  //创建多个写线程
  pthread_create(&id, NULL, pth_work, NULL);
  pthread_create(&id2, NULL, pth_work2, NULL);
   
  //等待子线程退出，回收其资源    
  pthread_join(id, NULL);
  pthread_join(id2, NULL);
  pthread_mutex_destroy(&lock); //销毁锁
  //sem_destroy(&sem);
  return 0;
}	