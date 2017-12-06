#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define SEM_W "sem_write"
#define SEM_R "sem_read"
#define SEM_ER "sem_enable_read"
#define ENABLE_NUM 3

void write_to_pipe(int fd[2], int sid);
int read_from_pipe(int fd[2]);
void P(sem_t *sem_ptr);
void V(sem_t *sem_ptr);
void catch_INT(int sig);
void destroy_sem();

int main(void)
{
    int fd[2];
    pid_t pid;
    int ret = -1, child_num = ENABLE_NUM, enable_val = -1, sid = 0, i;
    sem_t *write_psx, *read_psx, *enable_read_psx;
    write_psx = sem_open(SEM_W, O_CREAT, 0666, 1);
    read_psx = sem_open(SEM_R, O_CREAT, 0666, 0);
    //enable_read_psx = sem_open(SEM_ER, O_CREAT, 0666, 0);

    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(1);
    }
    for (i = 0; i < child_num; i++) {
        pid = fork();
        sid++;
        if (pid < 0) {
            printf("fork error\n");
            destroy_sem();
            exit(0);
        } else if (pid == 0) { // child process
            break;
        }
    }
    if (pid == 0) {
        P(write_psx);
        write_to_pipe(fd, sid);

        V(read_psx);
        V(write_psx);
        exit(0);

    } else {
        // only father could reach here
        signal(SIGINT, catch_INT);
        //sleep(1);
        while (1)
        {
            P(read_psx);
            read_from_pipe(fd);
        }
    }
    return 0;
}

void write_to_pipe(int fd[2], int sid)
{
    int value;
    char buf[100];
    close(fd[0]);
    memset(buf, sid + '0', 100);
    printf("child proc %d write %d bytes data\n", sid, (int)sizeof(buf));
    write(fd[1], buf, sizeof(buf));
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
    return ret;
}

void P(sem_t *sem_ptr)
{
    sem_wait(sem_ptr);
}

void V(sem_t *sem_ptr)
{
    sem_post(sem_ptr);
}

void catch_INT(int sig)
{
    printf("catch ctrl_c keydown\n");
    destroy_sem();
    exit(0);
}

void destroy_sem()
{
    sem_unlink(SEM_W);
    sem_unlink(SEM_R);
    //sem_unlink(SEM_ER);
}