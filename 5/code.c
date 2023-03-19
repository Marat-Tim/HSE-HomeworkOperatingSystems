#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define true (1)
#define false (0)
#define bool int

const int BUFFER_SIZE = 5000;

void process1(char input_path[], char output_name[]) {
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
    int output_fd;
    if ((output_fd = open(output_name, O_WRONLY)) < 0) {
        puts("[Process 1]: Can't open fifo for writing");
        exit(EXIT_FAILURE);
    }
    size_t write_size = write(output_fd, buffer, read_size + 1);
    if (write_size != read_size + 1) {
        puts("[Process 1]: Can't write a message to fifo");
        exit(EXIT_FAILURE);
    }
    if (close(output_fd) < 0) {
        puts("[Process 1]: Can't close fifo");
        exit(EXIT_FAILURE);
    }
}

void process2(char input_name[], char output_name[]) {
    int input_fd;
    if ((input_fd = open(input_name, O_RDONLY)) < 0) {
        puts("[Process 2]: Can't open fifo for reading");
        exit(EXIT_FAILURE);
    }
    char buffer[BUFFER_SIZE + 1];
    ssize_t read_size = read(input_fd, buffer, BUFFER_SIZE);
    if (read_size < 0) {
        puts("[Process 2]: Can't read data from fifo");
        exit(EXIT_FAILURE);
    }
    if (close(input_fd) < 0) {
        puts("[Process 2]: Can't close fifo");
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
    int output_fd;
    if ((output_fd = open(output_name, O_WRONLY)) < 0) {
        puts("[Process 2]: Can't open fifo for writing");
        exit(EXIT_FAILURE);
    }
    size_t write_size = write(output_fd, int_buffer, len);
    if (write_size != len) {
        puts("[Process 2]: Can't write a message to fifo");
        exit(EXIT_FAILURE);
    }
    if (close(output_fd) < 0) {
        puts("[Process 2]: Can't close fifo");
        exit(EXIT_FAILURE);
    }
}

void process3(char input_name[], char output_path[]) {
    int input_fd;
    if ((input_fd = open(input_name, O_RDONLY)) < 0) {
        puts("[Process 3]: Can't open fifo for reading");
        exit(EXIT_FAILURE);
    }
    char buffer[sizeof(int) + 1];
    ssize_t read_size = read(input_fd, buffer, sizeof(int) + 1);
    if (read_size < 0) {
        puts("[Process 3]: Can't read message from fifo");
        exit(EXIT_FAILURE);
    }
    if (close(input_fd) < 0) {
        puts("[Process 3]: Can't close fifo");
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
    char fifo1[] = "1.fifo", fifo2[] = "2.fifo";
    mknod(fifo1, S_IFIFO | 0666, 0);
    mknod(fifo2, S_IFIFO | 0666, 0);
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
        process1(argv[1], fifo1);
    } else if (pid2 == 0) {
        // Second fork
        process3(fifo2, argv[2]);
    } else {
        // Main process
        process2(fifo1, fifo2);
    }
}
