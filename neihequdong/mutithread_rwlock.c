//pth_rwlock.c	
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int counter; //用全局变量，让线程间共享数据

pthread_rwlock_t  rwlock; //读写锁

void *pth_write(void *arg)
{
  int t, i = (int)arg;
  while (1) {
    pthread_rwlock_wrlock(&rwlock); //上写锁（改写数据时，互斥所有)
              //上锁后，其它线程无法获得，将自旋忙等待(不睡眠)
    t = counter;
    usleep(1000);
    printf("write %d: %lu: counter=%d ++counter=%d\n", i, pthread_self(), t, ++counter); //pthread_self是获得线程ID
    pthread_rwlock_unlock(&rwlock); //解写锁
    usleep(10000);
  }
  return NULL;
}
void *pth_read(void *arg)
{
  int i = (int)arg;

  while (1) {
    pthread_rwlock_rdlock(&rwlock); //上读锁(除读读不互斥外，读写 写写要互斥)
    printf("read %d: %lu: %d\n", i, pthread_self(), counter); 
    pthread_rwlock_unlock(&rwlock); //解写锁
    usleep(2000);
  }
  return NULL;
}


int main(void)
{
  int i;
  pthread_t tid[6];
  pthread_rwlock_init(&rwlock, NULL); //构造读写锁

  //创建多个写线程
  for (i = 0; i < 2; i++){
    pthread_create(&tid[i], NULL, pth_write, (void *)i);
  }
    
  //创建多个读线程      
  for (i = 0; i < 4; i++){
    pthread_create(&tid[i+2], NULL, pth_read, (void *)i);
  }
}