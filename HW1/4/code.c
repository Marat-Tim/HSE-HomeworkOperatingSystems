#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define true (1)
#define false (0)
#define bool int

const int BUFFER_SIZE = 5000;

void process1(char input_path[], int output_fd) {
    char buffer[BUFFER_SIZE + 1];
    int input_fd = open(input_path, O_RDONLY);
    if (input_fd < 0) {
        puts("[Process 1]: Can't open the file");
        exit(EXIT_FAILURE);
    }
    ssize_t read_size = read(input_fd, buffer, BUFFER_SIZE);
    if (read_size == -1) {
        puts("[Process 1]: Can't read the data from the file");
        exit(EXIT_FAILURE);
    }
    buffer[read_size] = '\0';
    size_t write_size = write(output_fd, buffer, read_size + 1);
    if (write_size != read_size + 1) {
        puts("[Process 1]: Can't write a message to pipe");
        exit(EXIT_FAILURE);
    }
}

void process2(int input_fd, int output_fd) {
    char buffer[BUFFER_SIZE + 1];
    ssize_t read_size = read(input_fd, buffer, BUFFER_SIZE);
    if (read_size < 0) {
        puts("[Process 2]: Can't read data from pipe");
        exit(EXIT_FAILURE);
    }
    buffer[read_size] = '\0';
    int count = 0;
    bool is_number = false;
    for (int i = 0; i < read_size + 1; ++i) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            is_number = true;
        } else if (is_number) {
            ++count;
            is_number = false;
        }
    }
    char int_buffer[sizeof(int) + 1];
    int len = sprintf(int_buffer, "%d", count);
    size_t write_size = write(output_fd, int_buffer, len);
    if (write_size != len) {
        puts("[Process 2]: Can't write a message to pipe");
        exit(EXIT_FAILURE);
    }
}

void process3(int input_fd, char output_path[]) {
    char buffer[sizeof(int) + 1];
    ssize_t read_size = read(input_fd, buffer, sizeof(int) + 1);
    if (read_size < 0) {
        puts("[Process 3]: Can't read message from pipe");
        exit(EXIT_FAILURE);
    }
    int output_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (output_fd < 0) {
        puts("[Process 3]: Can't open file");
        exit(EXIT_FAILURE);
    }
    size_t write_size = write(output_fd, buffer, read_size);
    if (write_size != read_size) {
        puts("[Process 3]: Can't write data to file");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("Invalid number of command line arguments");
        return EXIT_FAILURE;
    }
    int pipe1_fd[2], pipe2_fd[2];
    if (pipe(pipe1_fd) < 0) {
        puts("Can't open pipe");
        return EXIT_FAILURE;
    }
    if (pipe(pipe2_fd) < 0) {
        puts("Can't open pipe");
        return EXIT_FAILURE;
    }
    int pid1, pid2;
    pid1 = fork();
    if (pid1 == -1) {
        puts("Failed to create first fork");
        return EXIT_FAILURE;
    }
    if (pid1 != 0) {
        pid2 = fork();
        if (pid2 == -1) {
            puts("Failed to create second fork");
            return EXIT_FAILURE;
        }
    }
    if (pid1 == 0) {
        // First fork
        process1(argv[1], pipe1_fd[1]);
    } else if (pid2 == 0) {
        // Second fork
        process3(pipe2_fd[0], argv[2]);
    } else {
        // Main process
        process2(pipe1_fd[0], pipe2_fd[1]);
    }
    if (close(pipe1_fd[0]) < 0 || close(pipe1_fd[1]) < 0 || close(pipe2_fd[0]) < 0 || close(pipe2_fd[1]) < 0) {
        puts("Can't close pipe");
        return EXIT_FAILURE;
    }
}
