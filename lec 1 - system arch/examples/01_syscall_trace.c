#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    /* malloc() is a user-space allocator API, not itself a system call. */
    char *buffer = malloc(4096);
    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    const char *message = "hello from user space\n";
    strcpy(buffer, message);

    /* These libc APIs eventually enter the kernel on Linux. */
    int fd = open("os-demo.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        free(buffer);
        return 1;
    }

    if (write(fd, buffer, strlen(buffer)) == -1) {
        perror("write");
    }

    /* Ask the kernel to push file data toward stable storage. */
    if (fsync(fd) == -1) {
        perror("fsync");
    }

    close(fd);
    free(buffer);
    return 0;
}
