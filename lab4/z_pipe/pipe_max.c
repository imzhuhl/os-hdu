#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void write_to_pipe(int fd[2]);

int main(void)
{
    int fd[2];
    pid_t pid;
    int ret = -1;

    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        exit(1);
    } else if (pid == 0) {
        write_to_pipe(fd);
    } else {
        wait(NULL);
    }

}

void write_to_pipe(int fd[2])
{
    int count, ret;
    char buffer[1024];
    close(fd[0]);
    memset(buffer, '*', 1024);

    ret = write(fd[1], buffer, sizeof(buffer));
    count = 1;
    while (ret != -1) {
        ret = write(fd[1], buffer, sizeof(buffer));
        count++;
        printf("pipe size: %d bytes\n", count * 1024);
    }
    printf("error: %s\n", strerror(errno));
}
