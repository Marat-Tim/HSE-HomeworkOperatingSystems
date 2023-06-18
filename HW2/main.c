#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "SharedMemory.h"

SharedMemory *not_processed;
SharedMemory *duty;
SharedMemory *therapist;
SharedMemory *surgeon;
SharedMemory *dentist;

void dutyMain(int number) {
    printf("Duty %d start working\n", number);
    while (true) {
        lock(&not_processed->buffer);
        if (not_processed->buffer.number == 0) {
            break;
        }
        unlock(&not_processed->buffer);
        int patient = readFrom(&duty->buffer);
        printf("Duty %d receive patient %d\n", number, patient);
        sleep(1 + rand() % 3);
        int random = rand() % 3;
        switch (random) {
            case 0:
                writeTo(&therapist->buffer, patient);
                printf("Send patient %d to therapist\n", patient);
                break;
            case 1:
                writeTo(&surgeon->buffer, patient);
                printf("Send patient %d to surgeon\n", patient);
                break;
            case 2:
                writeTo(&dentist->buffer, patient);
                printf("Send patient %d to dentist\n", patient);
                break;
            default:
                printf("switch case error");
                exit(EXIT_FAILURE);
        }
    }
}

void doctorMain(char* post, SharedMemory* shared_memory) {
    printf("%s start working\n", post);
    while (true) {
        lock(&not_processed->buffer);
        if (not_processed->buffer.number == 0) {
            break;
        }
        unlock(&not_processed->buffer);
        int patient = readFrom(&shared_memory->buffer);
        lock(&not_processed->buffer);
        not_processed->buffer.number--;
        unlock(&not_processed->buffer);
        printf("%s receive patient %d\n", post, patient);
        sleep(1 + rand() % 3);
        printf("Heal patient %d\n", patient);
    }

}

void patientMain(int number) {
    printf("Patient number %d has arrived\n", number);
    writeTo(&duty->buffer, number);
}

void unlinkAll() {
    unlinkSharedMemory(duty);
    unlinkSharedMemory(therapist);
    unlinkSharedMemory(surgeon);
    unlinkSharedMemory(dentist);
    unlinkSharedMemory(not_processed);
}

void deleteAll() {
    deleteSharedMemory(duty);
    deleteSharedMemory(therapist);
    deleteSharedMemory(surgeon);
    deleteSharedMemory(dentist);
    deleteSharedMemory(not_processed);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s *patient count*\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    not_processed = createSharedMemory("not_processed");
    not_processed->buffer.number = atoi(argv[1]);
    duty = createSharedMemory("duty");
    therapist = createSharedMemory("therapist");
    surgeon = createSharedMemory("surgeon");
    dentist = createSharedMemory("dentist");
    if (fork() == 0) {
        dutyMain(1);
        unlinkAll();
        return 0;
    }
    if (fork() == 0) {
        dutyMain(2);
        unlinkAll();
        return 0;
    }
    if (fork() == 0) {
        doctorMain("therapist", therapist);
        unlinkAll();
        return 0;
    }
    if (fork() == 0) {
        doctorMain("surgeon", surgeon);
        unlinkAll();
        return 0;
    }
    if (fork() == 0) {
        doctorMain("dentist", dentist);
        unlinkAll();
        return 0;
    }
    for (int i = 0; i < atoi(argv[1]); ++i) {
        if (fork() == 0) {
            patientMain(i);
            unlinkAll();
            return 0;
        }
    }
    deleteAll();
}