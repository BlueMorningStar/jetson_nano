/*同步的例子，生产者与消费者*/
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define NUM 5          

int queue[NUM];   //环形队列
sem_t free_num;   //空闲空间
sem_t use_num;    //已用空间

//生产者
void *producer(void *arg)
{
  int i = 0;

  while (1) {
      sem_wait(&free_num);       //空闲空间减一
      queue[i] = rand() % 1000 + 1; //模拟生产一个产品(随机数)
      printf("----Produce---%d\n", queue[i]);        
      sem_post(&use_num);        //已用空间加一

      i = (i+1) % NUM;             //下标求余方式，实现环形
  }
}

//消费者
void *consumer(void *arg)
{
  int i = 0;

  while (1) {
    sem_wait(&use_num);      //已用空间减一
    printf("-Consume---%d\n", queue[i]);
    queue[i] = 0;              //模拟消费一个产品，
    sem_post(&free_num);    //消费掉以后,空闲空间加一
    i = (i+1) % NUM;
  }
}

int main(int argc, char *argv[])
{
  pthread_t pid, cid;

  sem_init(&free_num, 0, NUM);  //初始化空闲空间为5
  sem_init(&use_num, 0, 0);     //已用空间为0

  pthread_create(&pid, NULL, producer, NULL);
  pthread_create(&cid, NULL, consumer, NULL);

  pthread_join(pid, NULL);
  pthread_join(cid, NULL);

  sem_destroy(&free_num);
  sem_destroy(&use_num);

  return 0;
}	