#include "init.h"

key_t key;
int shmid;
void *shmp;

sem_t *send_psx;
sem_t *recv_psx;
sem_t *final_psx;

void init();
void destroy();
void catch_INT(int sig);
int check_value(sem_t *semtmp, int style);
int main(void)
{
	int ret = -1;
    char input[SHM_SIZE];
    char info[SHM_SIZE];
    signal(SIGINT, catch_INT);
    init();

    memset(input, '\0', sizeof(input));
    // send msg to receiver
    while (1) {
        printf("please input: ");
        scanf("%s", input);

        P(send_psx);
        strcpy(info, (char *)shmp);
        strcat(info, input);
        strcpy((char *)shmp, info);
        if (strcmp(input, "exit") == 0) {
            ret = check_value(recv_psx, 0);
            if (ret == 1) {
                V(recv_psx);
            }
            break;
        }
        memset(input, '\0', sizeof(input));
        ret = check_value(recv_psx, 0);
        if (ret == 1) {
            V(recv_psx);
        }
        V(send_psx);
    }

    // recv end signal
    P(final_psx);
    strcpy(input, (char *)shmp);
    printf("sender recv: %s\n", input);
    destroy();
    printf("sender end\n");
    return 0;
}

void init()
{
    key = KEY_NUM;
    shmid = shmget(key, SHM_SIZE, 0666|IPC_CREAT);
    if (shmid < 0) {
        printf("shmget error\n");
        exit(1);
    }
    shmp = shmat(shmid, NULL, 0);
    memset((char *)shmp, '\0', SHM_SIZE);
    send_psx = sem_open(SEM_SEND, O_CREAT, 0666, 1);
    recv_psx = sem_open(SEM_RECV, O_CREAT, 0666, 0);
    final_psx = sem_open(SEM_FIN, O_CREAT, 0666, 0);
}
void destroy()
{
    shmdt(shmp);
    shmctl(shmid, IPC_RMID, NULL);
    sem_unlink(SEM_SEND);
    sem_unlink(SEM_RECV);
    sem_unlink(SEM_FIN);
}
int check_value(sem_t *semtmp, int style)
{
    // return 0: can not release this sem
    // return 1: you should release it
    // style means we check value with different ways
    int ret = -1, sem_vl;
    ret = sem_getvalue(semtmp, &sem_vl);
    if (ret == -1) {
        printf("get sem value error\n");
        destroy();
        exit(0);
    }
    if (style == 0) {
        if (sem_vl == 0)
            return 1;
        else
            return 0;
    }
}

void catch_INT(int sig)
{
    printf("catch SIGINT\n");
    destroy();
    exit(0);
}

