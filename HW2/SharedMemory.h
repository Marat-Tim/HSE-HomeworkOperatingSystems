#ifndef HSE_HOMEWORKOPERATINGSYSTEMS_SHAREDMEMORY_H
#define HSE_HOMEWORKOPERATINGSYSTEMS_SHAREDMEMORY_H

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "ThreadSafeBuffer.h"

typedef struct SharedMemoryStruct {
    int id;
    ThreadSafeBuffer buffer;
    char name[100];
} SharedMemory;

SharedMemory *createSharedMemory(char *name) {
    int id = shm_open(name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    ftruncate(id, sizeof(SharedMemory));
    SharedMemory *shared_memory =
            (SharedMemory *) mmap(0, sizeof(SharedMemory), PROT_WRITE | PROT_READ, MAP_SHARED, id, 0);
    shared_memory->id = id;
    strncpy(shared_memory->name, name, 100);
    init(&shared_memory->buffer);
    return shared_memory;
}

void unlinkSharedMemory(SharedMemory* shared_memory) {
    if (munmap(shared_memory, sizeof(SharedMemory)) == -1) {
        perror("munmap() failed");
        exit(EXIT_FAILURE);
    }
}

void deleteSharedMemory(SharedMemory* shared_memory) {
    char name[100];
    strncpy(name, shared_memory->name, 100);
    unlinkSharedMemory(shared_memory);
    shm_unlink(name);
}

#endif //HSE_HOMEWORKOPERATINGSYSTEMS_SHAREDMEMORY_H
