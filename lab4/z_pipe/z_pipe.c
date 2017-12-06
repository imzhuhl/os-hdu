#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_W "sem_write"
#define SEM_R "sem_read"
#define SEM_ER "sem_enable_read"
#define ENABLE_NUM 3

void write_to_pipe(int fd[2]);
int read_from_pipe(int fd[2]);

int main(void)
{
    int fd[2];
    pid_t pid;
    char buffer[64];
    int ret = -1, child_num = ENABLE_NUM, write_val = -1;
    sem_t *write_psx, *read_psx, *enable_read_psx;
    write_psx = sem_open(SEM_W, O_CREAT, 0666, 1);
    read_psx = sem_open(SEM_R, O_CREAT, 0666, 0);
    enable_read_psx = sem_open(SEM_ER, O_CREAT, 0666, 0);

    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(1);
    }
    while (child_num > 0) {
        pid = fork();
        if (pid < 0) {
            printf("fork error\n");
            exit(1);
        } else if (pid == 0) { // child process
            sem_wait(write_psx);

            write_to_pipe(fd);

            sem_post(enable_read_psx);
            sem_getvalue(enable_read_psx, &write_val);
            if (write_val == ENABLE_NUM) {
                sem_post(read_psx);
            }

            sem_post(write_psx);
            exit(0);
        } else { // father process
            child_num--;
        }
    }
    // only father could reach here
    sem_wait(read_psx);

    read_from_pipe(fd);

    sem_unlink(SEM_W);
    sem_unlink(SEM_R);
    sem_unlink(SEM_ER);
    return 0;
}

void write_to_pipe(int fd[2])
{
    int value, ret = -1;
    close(fd[0]);
    char buf[1024];
    memset(buf, '*', 1024);
    buf[1023] = '$';
    ret = write(fd[1], buf, sizeof(buf));
    printf("child write %d bytes data\n", ret);
}

int read_from_pipe(int fd[2])
{
    int ret;
    char buf[1024];
    close(fd[1]);
    memset(buf, '\0', 1024);
    ret = read(fd[0], buf, 1024);
    while (ret > 0) {
        printf("father read %d bytes data: %s\n", ret, buf);
        memset(buf, '\0', 1024);
        ret = read(fd[0], buf, 1024);
    }
    return 0;
}
