#include "init.h"

key_t key;
int shmid;
void *shmp;

sem_t *send_psx;
sem_t *recv_psx;

void init();
void destroy();

int main(void)
{
	char input[SHM_SIZE];
	init();
	printf("please input: ");
	scanf("%s", input);
	
	// send msg to receiver
	P(send_psx);
	strcpy((char *)shmp, input);
	V(recv_psx);
	
	// recv end signal
	P(send_psx);
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
	send_psx = sem_open(SEM_SEND, O_CREAT, 0666, 1);
	recv_psx = sem_open(SEM_RECV, O_CREAT, 0666, 0);
}
void destroy()
{
	sem_close(send_psx);
	sem_close(recv_psx);
	sem_unlink(SEM_SEND);
	sem_unlink(SEM_RECV);
}
