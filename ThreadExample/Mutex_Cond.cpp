#include <iostream>
#include <queue>
#include <cstdlib>

#include <unistd.h>
#include <pthread.h>

using namespace std;

struct{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    queue<int> product;
}sharedData = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

void * produce(void *ptr)
{
    for (int i = 0; i < 10; ++i)
    {
            pthread_mutex_lock(&sharedData.mutex);  //生产的时候将product锁住

            sharedData.product.push(i);

            pthread_mutex_unlock(&sharedData.mutex);

            if (sharedData.product.size() == 1)     //生产了一个产品则唤醒消费线程
                pthread_cond_signal(&sharedData.cond);
         }
}

void * consume(void *ptr)
{
    for (int i = 0; i < 10;)
    {
            pthread_mutex_lock(&sharedData.mutex);

            while(sharedData.product.empty())  //消费完了将进程放入睡眠状态
                pthread_cond_wait(&sharedData.cond, &sharedData.mutex);

            ++i;
            cout<<"consume:"<<sharedData.product.front()<<endl;
            sharedData.product.pop();

            pthread_mutex_unlock(&sharedData.mutex);
      }
}

int main()
{
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, consume, NULL);
    pthread_create(&tid2, NULL, produce, NULL);

    void *retVal;

    pthread_join(tid1, &retVal);
    pthread_join(tid2, &retVal);

    return 0;
}
