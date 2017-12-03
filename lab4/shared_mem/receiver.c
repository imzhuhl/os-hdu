#include "init.h"

key_t key;
int shmid;
void *shmp;

sem_t *send_psx;
sem_t *recv_psx;

void init();

int main(void)
{
	char input[SHM_SIZE];
	init();
	// recv msg from sender
	P(recv_psx);
	strcpy(input, (char *)shmp);
	printf("recv msg: %s\n", input);
	strcpy((char *)shmp, "over");
	V(send_psx);
	
	printf("receiver end\n");
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

