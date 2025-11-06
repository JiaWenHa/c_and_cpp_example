#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *async_task(void *arg){
    printf("async task start...\n");
    sleep(2);
    printf("async task complete!\n");
    return NULL;
}

int main() {
    pthread_t tid;

    printf("main thread: start async task\n");
    pthread_create(&tid, NULL, &async_task, NULL);

    printf("main thread exec other works...\n");

    sleep(3);

    pthread_join(tid, NULL);
    printf("🔚 程序结束\n");

    return 0;
}