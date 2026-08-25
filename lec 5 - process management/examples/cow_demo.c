// Lecture-equivalent Copy-on-Write demo (not the instructor's original source).
// Linux only: inspect each process with /proc/<pid>/smaps while it runs.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE (10 * 1024 * 1024)   // 10 MiB
#define PAGE 4096

int main(void) {
    char *buffer = malloc(SIZE);
    if (!buffer) {
        perror("malloc");
        return 1;
    }

    // Touch every page so the allocation becomes physically backed.
    memset(buffer, 0, SIZE);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        free(buffer);
        return 1;
    }

    if (pid > 0) {
        // Parent: mostly read-only so pages can stay shared.
        printf("parent pid=%d child pid=%d\n", getpid(), pid);
        fflush(stdout);
        for (;;) {
            volatile char value = buffer[0];
            (void)value;
            sleep(1);
        }
    } else {
        // Child: dirty one byte in a new page every 200 ms.
        // Watch Private_Dirty grow and shared memory shrink in smaps.
        printf("child pid=%d\n", getpid());
        fflush(stdout);
        for (size_t i = 0; i < SIZE; i += PAGE) {
            buffer[i]++;
            usleep(200000);
        }
        for (;;) sleep(1);
    }
}
