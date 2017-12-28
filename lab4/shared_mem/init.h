#ifndef _INIT_H_
#define _INIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>  
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024
#define KEY_NUM 1111

#define SEM_SEND "sem_send"
#define SEM_RECV "sem_recv"
#define SEM_FIN  "sem_final"

void P(sem_t *semp);
void V(sem_t *semp);

#endif
