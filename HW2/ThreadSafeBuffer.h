#ifndef HSE_HOMEWORKOPERATINGSYSTEMS_THREADSAFEBUFFER_H
#define HSE_HOMEWORKOPERATINGSYSTEMS_THREADSAFEBUFFER_H

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct ThreadSafeBufferStruct {
    sem_t semaphore_read;
    sem_t semaphore_write;
    int number;
} ThreadSafeBuffer;

void init(ThreadSafeBuffer *buffer) {
    sem_init(&buffer->semaphore_read, 1, 0);
    sem_init(&buffer->semaphore_write, 1, 1);
}

void writeTo(ThreadSafeBuffer *buffer, int number) {
    sem_wait(&buffer->semaphore_write);
    buffer->number = number;
    sem_post(&buffer->semaphore_read);
}

int readFrom(ThreadSafeBuffer *buffer) {
    sem_wait(&buffer->semaphore_read);
    int number = buffer->number;
    sem_post(&buffer->semaphore_write);
    return number;
}

void lock(ThreadSafeBuffer *thread_safe_buffer) {
    sem_wait(&thread_safe_buffer->semaphore_write);
}

void unlock(ThreadSafeBuffer *thread_safe_buffer) {
    sem_post(&thread_safe_buffer->semaphore_write);
}

#endif //HSE_HOMEWORKOPERATINGSYSTEMS_THREADSAFEBUFFER_H
