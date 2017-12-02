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

sem_t send_psx, recv_psx;
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
    char info[100];
    struct msgbuf s_msg;
    s_msg.mtype = 1;
    while (1) {
        sem_wait(&send_psx);

        printf("send_thread: ");
        scanf("%s", info);
        if (strcmp(info, "exit") == 0) {
            strcpy(s_msg.mtext, "end");
            msgsnd(msgid, &s_msg, sizeof(struct msgbuf), 0);

            sem_post(&recv_psx);
            break;
        }
        strcpy(s_msg.mtext, info);
        msgsnd(msgid, &s_msg, sizeof(struct msgbuf), 0);

        sem_post(&recv_psx);
    }
    sem_wait(&send_psx);
    msgrcv(msgid, &s_msg, sizeof(struct msgbuf), 1, 0);
    printf("send_thread recv: %s\ndone.\n", s_msg.mtext);
    // remove msg queue
    msgctl(msgid, IPC_RMID, 0);
    pthread_exit(NULL);
}

void *recv_thread(void *arg)
{
    struct msgbuf r_msg;
    r_msg.mtype = 1;
    while (1) {
        sem_wait(&recv_psx);

        msgrcv(msgid, &r_msg, sizeof(struct msgbuf), 1, 0);
        if (strcmp(r_msg.mtext, "end") == 0) {
            strcpy(r_msg.mtext, "over");
            msgsnd(msgid, &r_msg, sizeof(struct msgbuf), 0);
            printf("recv_thread recv 'end', return 'over'\n");

            sem_post(&send_psx);
            break;
        }
        printf("recv_thrend: %s\n", r_msg.mtext);

        sem_post(&send_psx);
    }
    pthread_exit(NULL);
}
