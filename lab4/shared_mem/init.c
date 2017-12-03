#include "init.h"

void P(sem_t *semp)
{
	sem_wait(semp);
}
void V(sem_t *semp)
{
	sem_post(semp);
}
