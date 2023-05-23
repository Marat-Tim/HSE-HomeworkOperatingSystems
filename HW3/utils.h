#ifndef IHW3__UTILS_H_
#define IHW3__UTILS_H_

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t write_mutex;

bool is_init = false;

void putsSync(char* string) {
    // very good reshenie
    if (!is_init) {
        pthread_mutex_init(&write_mutex, NULL);
        is_init = true;
    }
    pthread_mutex_lock(&write_mutex);
    puts(string);
    pthread_mutex_unlock(&write_mutex);
}

void dieWithError(char *message) {
    putsSync(message);
    exit(1);
}

bool containsMinusOne(const int array[], int size) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == -1) {
            return true;
        }
    }
    return false;
}

#endif//IHW3__UTILS_H_
