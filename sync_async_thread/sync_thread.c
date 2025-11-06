#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *sync_task(void *arg) {
    printf("sync task start running...\n");
    sleep(2); // 模拟耗时操作
    printf("sync task finished\n");
    return (void*)42;
}

int main() {
    pthread_t tid;
    void *result; 

    printf(" main thread: start sync task\n");
    pthread_create(&tid, NULL, &sync_task, NULL);

    // 主线程阻塞，等待子线程结束（同步）
    pthread_join(tid, &result);

    printf("main thread: get result %ld\n", (long)result);
    printf("main thread continue.\n");
    return 0;
}