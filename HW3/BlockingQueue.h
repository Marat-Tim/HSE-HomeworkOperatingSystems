#ifndef IHW3__BLOCKINGQUEUE_H_
#define IHW3__BLOCKINGQUEUE_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 100

struct BlockingQueue {
    char *data[MAX_QUEUE_SIZE];
    pthread_mutex_t mutex;
    sem_t sem;
    int size;
};

void initBlockingQueue(struct BlockingQueue *blocking_queue) {
    pthread_mutex_init(&blocking_queue->mutex, NULL);
    sem_init(&blocking_queue->sem, 0, 0);
    blocking_queue->size = 0;
}

void addToBlockingQueue(struct BlockingQueue *blocking_queue, char *value) {
    puts("lock mutex");
    pthread_mutex_lock(&blocking_queue->mutex);
    blocking_queue->data[blocking_queue->size] = value;
    ++blocking_queue->size;
    puts("post sem");
    sem_post(&blocking_queue->sem);
    pthread_mutex_unlock(&blocking_queue->mutex);
    puts("unlock mutex");
}

char *getFromBlockingQueue(struct BlockingQueue *blocking_queue) {
    puts("before sem_wait");
    sem_wait(&blocking_queue->sem);
    puts("after sem_wait\nlock mutex");
    pthread_mutex_lock(&blocking_queue->mutex);
    char *ret_value = blocking_queue->data[0];
    for (int i = 0; i < blocking_queue->size - 2; ++i) {
        blocking_queue->data[i] = blocking_queue->data[i + 1];
    }
    --blocking_queue->size;
    pthread_mutex_unlock(&blocking_queue->mutex);
    puts("unlock mutex");
    return ret_value;
}

#endif// IHW3__BLOCKINGQUEUE_H_
