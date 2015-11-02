#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>

#include <sys/syscall.h>
#include <sys/types.h>
#define MAX_THREAD 3

pthread_barrier_t barrier;	//

void* running_print(void *arg) {
	int mid = *(int *) arg;
	int s = 0;
	if (*((int *) arg + 1)) {
		cpu_set_t mask;
		
		CPU_ZERO(&mask);
		CPU_SET(0, &mask);
		s = sched_setaffinity(0, sizeof(mask), &mask);

		struct sched_param param;
		printf("Thread %d sched_setscheduler()\n", mid);
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		s = sched_setscheduler(0, SCHED_FIFO, &param);
		printf("Thread %d sched after %s\n", mid, strerror(errno));
	}
	//	for (int i = 0; i < 100000000; i++);
	pthread_barrier_wait(&barrier);
	printf("ERROR %s\n", strerror(errno));
	for (int it = 0; it < 8; it++) {
		printf("Thread %d is running\n", mid);
		// busy 1 second
		double a = 0;
		for (int i = 0; i < 10000000; i++) {
			a += 0.1f;
		}
	}
	return (void *) 0;
}
void* create_arg(int x, int scheduler) {
	int *p = (int *) malloc(sizeof(int) * 2);
	*p = x, *(p+1) = scheduler;
	return (void *) p;
}
int main(int argc, char *argv[]) {
	int scheduler = 0;	// default scheduler
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "SCHED_FIFO")) {
			scheduler = 1;
		}
	}

	// int s = 0;
	// struct sched_param param;
	// printf("Thread main sched_setscheduler()\n");
	// param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	// s = sched_setscheduler(0, SCHED_FIFO, &param);
	// printf("%s\n", strerror(errno));
	// assert(s == 0);

	pthread_t tid[MAX_THREAD];

	pthread_barrier_init(&barrier, NULL, MAX_THREAD);
	
	for (int i = 0; i < MAX_THREAD; i++) {
		printf("Thread %d was created.\n", i+1);
		int err = pthread_create(&tid[i], NULL, running_print, create_arg(i+1, scheduler));
		assert(err == 0);
	}
	for (int i = 0; i < MAX_THREAD; i++)
		pthread_join(tid[i], NULL);
	return 0;
}
