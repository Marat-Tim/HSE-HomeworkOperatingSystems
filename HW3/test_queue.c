#include "BlockingQueue.h"
#include <stdio.h>
#include <pthread.h>

struct BlockingQueue blocking_queue;

void* thread1(void* arg) {
    puts("start getting");
    char* a = getFromBlockingQueue(&blocking_queue);
    puts("end getting");
    printf("[%s]", a);
    puts("write text");
    return NULL;
}

void* thread2(void* arg) {
    char* a = "423432";
    addToBlockingQueue(&blocking_queue, a);
    return NULL;
}

int main() {
    initBlockingQueue(&blocking_queue);
    pthread_t pthread[10];
    for (int i = 0; i < 10; ++i) {
        pthread_create(pthread + i, NULL, thread1, NULL);
    }
    pthread_t t1;
    pthread_create(&t1, NULL, thread2, NULL);
    pthread_join(t1, NULL);
    for (int i = 0; i < 10; ++i) {
        pthread_join(pthread[i], NULL);
    }
}