/**
 * 示例代码：生产者-消费者模型
 * 共享数据: buffer, count, produce_idx, consume_idx 是多个线程共享的数据，需要 mutex 保护。
 * 两个条件变量:
 *      cond_not_full: 当 count < BUFFER_SIZE 时，生产者可以继续生产。
 *      cond_not_empty: 当 count > 0 时，消费者可以继续消费。
 * 
 * 生产者 (producer）:
 *      1. 获取锁。
 *      2. 检查 count == BUFFER_SIZE。如果是，则调用 pthread_cond_wait(&cond_not_full, &mutex) 释放锁并等待。当被唤醒且重新获得锁后，再次检查条件。
 *      3. 生产数据，更新 buffer, produce_idx, count。
 *      4. 调用 pthread_cond_signal(&cond_not_empty) 通知可能在等待的消费者。
 *      5. 释放锁。
 * 
 * 消费者 (consumer):
 *      1. 获取锁。
 *      2. 检查 count == 0。如果是，则调用 pthread_cond_wait(&cond_not_empty, &mutex) 释放锁并等待。当被唤醒且重新获得锁后，再次检查条件。
 *      3. 消费数据，更新 buffer, consume_idx, count。
 *      4. 调用 pthread_cond_signal(&cond_not_full) 通知可能在等待的生产者。
 *      5. 释放锁。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int count = 0; // 缓冲区中当前元素的数量
int produce_idx = 0; // 生产者写入位置
int consume_idx = 0; // 消费者读取位置

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER; // 缓冲区不满的条件
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER; // 缓冲区不空的条件

void *producer(void *arg){
    int item;
    for (int i = 0; i < NUM_ITEMS; i++) {
        item = rand() % 100; // 生产一个随机数

        // 等待直到缓冲区不满
        while (count == BUFFER_SIZE)
        {
            printf("Producer: Buffer is full. Waiting...\n");
            // 释放互斥锁，线程进入睡眠状态，加入到条件变量的等待队列中
            pthread_cond_wait(&cond_not_full, &mutex);
        }

        // 生产数据
        buffer[produce_idx] = item;
        produce_idx = (produce_idx + 1) % BUFFER_SIZE;
        count++;
        printf("Producer: Produced %d. Count = %d", item, count);

        // 通知消费者缓冲区不空了
        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&mutex);

        // 模拟生产时间
        usleep(100000); // 100ms
    }
    return NULL;
}

void *consumer(void *arg){
    int item;
    for(int i = 0; i < NUM_ITEMS; i++){
        pthread_mutex_lock(&mutex);

        // 等待直到缓冲区不空
        while (count == 0)
        {
            printf("Consumer: Buffer is empty. Waiting...\n");
            pthread_cond_wait(&cond_not_empty, &mutex);
        }

        // 消费数据
        item = buffer[consume_idx];
        consume_idx = (consume_idx + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer: Consumed %d. Count = %d\n", item, count);

        // 通知生产者缓冲区不满
        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&mutex);

        // 模拟消费时间
        usleep(150000); // 150ms
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;

    srand(time(NULL)); // 初始化随机数种子

    if(pthread_create(&prod_thread, NULL, producer, NULL) != 0){
        perror("pthread_create consumer");
        exit(1);
    }

    if(pthread_create(&cons_thread, NULL, consumer, NULL) != 0){
        perror("pthread_create consumer");
        exit(1);
    }

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // 清理资源
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_not_full);
    pthread_cond_destroy(&cond_not_empty);

    printf("Main: Program completed.\n");
    return 0;
}
