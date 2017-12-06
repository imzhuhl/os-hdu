#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void *send_thread(void *arg);
void *recv_thread(void *arg);
void P(sem_t *sem_ptr);
void V(sem_t *sem_ptr);
void catch_INT(int sig);

sem_t send_psx, recv_psx, final_recv;
pthread_t send_pid, recv_pid;

struct msgbuf
{
    long mtype;
    char mtext[256];
};
int msgid;

int main(void)
{
    sem_init(&send_psx, 0, 1);
    sem_init(&recv_psx, 0, 0);
    sem_init(&final_recv, 0, 0);
    msgid = msgget(IPC_PRIVATE, 0666|IPC_CREAT);
    if (msgid < 0) {
        printf("msgget error\n");
        exit(1);
    }
    pthread_create(&send_pid, NULL, send_thread, NULL);
    pthread_create(&recv_pid, NULL, recv_thread, NULL);

    pthread_join(send_pid, NULL);
    pthread_join(recv_pid, NULL);

    return 0;
}

void *send_thread(void *arg)
{
    int count = 0;
    char info[256];
    struct msgbuf s_msg;
    s_msg.mtype = 1;
    while (1) {
        P(&send_psx);

        printf("send_thread: ");
        scanf("%s", info);
        // memset(info, '*', sizeof(info));
        if (strcmp(info, "exit") == 0) {
            strcpy(s_msg.mtext, "end");
            msgsnd(msgid, &s_msg, sizeof(struct msgbuf), 0);
            V(&recv_psx);
            break;
        }
        strcpy(s_msg.mtext, info);
        count++;
        printf("%d\n", count);
        msgsnd(msgid, &s_msg, sizeof(struct msgbuf), 0);

        V(&send_psx);
        V(&recv_psx);
    }
    P(&final_recv);
    msgrcv(msgid, &s_msg, sizeof(struct msgbuf), 2, 0);
    printf("send_thread recv: %s\ndone.\n", s_msg.mtext);
    // remove msg queue
    msgctl(msgid, IPC_RMID, 0);
    pthread_exit(NULL);
}

void *recv_thread(void *arg)
{
    sleep(10);
    struct msgbuf r_msg;
    r_msg.mtype = 1;
    while (1) {
        P(&recv_psx);

        msgrcv(msgid, &r_msg, sizeof(struct msgbuf), 1, 0);
        if (strcmp(r_msg.mtext, "end") == 0) {
            strcpy(r_msg.mtext, "over");
            r_msg.mtype = 2;
            msgsnd(msgid, &r_msg, sizeof(struct msgbuf), 0);
            printf("recv_thread recv 'end', return 'over'\n");

            V(&final_recv);
            break;
        }
        printf("recv_thrend: %s\n", r_msg.mtext);

    }
    pthread_exit(NULL);
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
    printf("catch SIGINT\n");
    msgctl(msgid, IPC_RMID, 0);
    exit(0);
}