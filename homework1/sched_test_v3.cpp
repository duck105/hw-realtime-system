#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>	// struct timespace, clock_gettime
#include <assert.h>

#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>

#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#define MAX_THREAD 2

#define handle_error_en(en, msg) \  
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)  

static void display_pthread_attr(pthread_attr_t *attr, const char *prefix)  
{  
    int s, i;  
    size_t v;  
    void *stkaddr;  
    struct sched_param sp;  
  
    s = pthread_attr_getdetachstate(attr, &i);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getdetachstate");  
    printf("%sDetach state        = %s\n", prefix,  
        (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :  
        (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :  
        "???");  
  
    s = pthread_attr_getscope(attr, &i);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getscope");  
    printf("%sScope               = %s\n", prefix,  
        (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :  
        (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :  
        "???");  
  
    s = pthread_attr_getinheritsched(attr, &i);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getinheritsched");  
    printf("%sInherit scheduler   = %s\n", prefix,  
        (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :  
        (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :  
        "???");  
  
    s = pthread_attr_getschedpolicy(attr, &i);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getschedpolicy");  
    printf("\033[3;31m%sScheduling policy   = %s\033[0m\n", prefix,  
        (i == SCHED_OTHER) ? "SCHED_OTHER" :  
        (i == SCHED_FIFO)  ? "SCHED_FIFO" :  
        (i == SCHED_RR)    ? "SCHED_RR" :  
        "???");  
  
    s = pthread_attr_getschedparam(attr, &sp);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getschedparam");  
    printf("\033[3;31m%sScheduling priority = %d\033[0m\n", prefix, sp.sched_priority);  
  
    s = pthread_attr_getguardsize(attr, &v);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getguardsize");  
    printf("%sGuard size          = %d bytes\n", prefix, v);  
  
    s = pthread_attr_getstack(attr, &stkaddr, &v);  
    if (s != 0)  
        handle_error_en(s, "pthread_attr_getstack");  
    printf("%sStack address       = %p\n", prefix, stkaddr);  
    printf("%sStack size          = 0x%x bytes\n", prefix, v);  
}  

static double my_clock(void) {
	struct timespec t;
	assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
	return 1e-9 * t.tv_nsec + t.tv_sec;
}
static void busy_work(int mid) {
//	pthread_mutex_lock(&outputlock);
	{
		double sttime = my_clock();
		while (1) {
			if (my_clock() - sttime > 1)
				break;
		}
		printf("Thread %d is running\n", mid);
	}
//	pthread_mutex_unlock(&outputlock);
}
static void* running_print(void *arg) {
	int mid = *(int *) arg;
	int s = 0;

	// enforce them work on single CPU 0
	// set only 0, otherwise, add new line: CPU_SET(1, &mask); let it work on CPU 0 and CPU 1
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	s = sched_setaffinity(0, sizeof(mask), &mask);

	pthread_attr_t attr;
	assert(pthread_getattr_np(pthread_self(), &attr) == 0);
	
	printf("\033[1;32m\tThread %d information \033[0m\n", mid);
	display_pthread_attr(&attr, "\t");
	for (int it = 0; it < 3; it++)
		busy_work(mid);
	return (void *) 0;
}
static void* create_arg(int x, int scheduler) {
	int *p = (int *) malloc(sizeof(int) * 2);
	*p = x, *(p+1) = scheduler;
	return (void *) p;
}
int main(int argc, char *argv[]) {
	int scheduler = 0;	// default scheduler
	int s = 0;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "SCHED_FIFO")) {
			scheduler = 1;
		}
	}

	pthread_t tid[MAX_THREAD];
	
	for (int i = 0; i < MAX_THREAD; i++) {
		pthread_attr_t *attr = new pthread_attr_t;
		struct sched_param *param = new struct sched_param;
		if (scheduler == 1) {
			// ASC ORDER
			param->sched_priority = sched_get_priority_max(SCHED_FIFO) - i;
			// DES ORDER
//			param->sched_priority = sched_get_priority_min(SCHED_FIFO) + i;
			// set scheduler
			pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
			pthread_attr_setschedpolicy(attr, SCHED_FIFO);
			pthread_attr_setschedparam(attr, param);
			printf("\tCREATE ERROR MSG: %s\n", strerror(errno));
			printf("Thread %d was created.\n", i+1);
			assert(pthread_create(&tid[i], attr, running_print, create_arg(i+1, scheduler)) == 0);
		} else {
			printf("Thread %d was created.\n", i+1);
			assert(pthread_create(&tid[i], NULL, running_print, create_arg(i+1, scheduler)) == 0);
		}
	}
	for (int i = 0; i < MAX_THREAD; i++)
		pthread_join(tid[i], NULL);
	return 0;
}
