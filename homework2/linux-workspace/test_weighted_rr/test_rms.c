#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <sched.h>
#include <getopt.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>

#define SCHED_NORMAL 0
#define SCHED_WEIGHTED_RR 6

#define SYS_weighted_rr_getquantum 337
#define SYS_weighted_rr_setquantum 338
#define SYS_RMS_getperiod 339
#define SYS_RMS_setperiod 340
#define START_CHAR 97

struct thread_args {
  int tid;
  int prio;
  int nchars;
  char mychar;
  int exeTime;
};

int sched_policy, quantum, old_quantum, num_threads, buffer_size;
int total_num_chars;
char *val_buf;
int *tid_buf;
int val_buf_pos = 0;
pthread_t *threads;

static double my_clock(void) {
	struct timespec t;
	assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
	return 1e-9 * t.tv_nsec + t.tv_sec;
}
void fail(char* msg)
{
	printf("%s\n", msg);
	exit(-1);
}

void *run(void *arg) 
{
	int i;
	struct thread_args *my_args = (struct thread_args*) arg;
	
	//+ write characters to the val_bufs
	double sttime = my_clock();
	double time_tick = 0;
	while (1) {
		if (my_clock() - sttime > my_args->exeTime)
			break;
		double tmp = my_clock() - sttime;
		tmp = floor(tmp * 1000);
		if (tmp > time_tick) {
			time_tick = tmp;
			//printf("tick %c%d %lf\n", my_args->mychar, my_args->tid, tmp);
			if (val_buf_pos > total_num_chars)
				break;
			*(tid_buf + val_buf_pos) = my_args->tid;
			*(val_buf + val_buf_pos) = my_args->mychar;
			__sync_fetch_and_add(&val_buf_pos,1);
			
		}
	}
	//printf("lst %lf\n", time_tick);
	//fflush(stdout);
	free(my_args);
	pthread_exit(NULL);
}

int gcd(int x, int y) {
	int t;
	while (x%y)
		t = x, x = y, y = t % y;
	return y;
}
int main(int argc, char *argv[])
{
	struct sched_param param;
	struct thread_args *targs;
	pthread_attr_t attr;
	int i,j;
	char cur;
	srand(time(NULL));	
	if( argc != 5 )
		fail("Invalid arguments count");
	
	//+ parse arguments
	if( strcmp(argv[1], "default") == 0 )
		sched_policy = SCHED_NORMAL;
	else if( strcmp(argv[1], "weighted_rr") == 0 )
		sched_policy = SCHED_WEIGHTED_RR;
	else
		fail("Invalid scheduling policy");
	
	quantum = atoi(argv[2]);
	num_threads = atoi(argv[3]);
	buffer_size = atoi(argv[4]);

	printf("sched_policy: %d, quantum: %d, num_threads: %d, buffer_size: %d\n", sched_policy, quantum, num_threads, buffer_size);
	
	//+ set weighted rr scheduling policy
	if (sched_policy == SCHED_WEIGHTED_RR)
	{
		param.sched_priority = 0;
		if ( sched_setscheduler(getpid(), sched_policy, &param) == -1)
		{
			perror("sched_setscheduler");
			fail("sched_setscheduler fail");
		};

		old_quantum = syscall (SYS_weighted_rr_getquantum);
		syscall (SYS_weighted_rr_setquantum, quantum);
   	}
	
	//+ create the buffer
	if ( (val_buf = (char *) malloc(buffer_size)) == NULL )
		fail("malloc(buffer_size) fail");
	if ( (tid_buf = (int *) malloc(buffer_size*sizeof(int))) == NULL )
		fail("malloc(buffer_size) fail");
	total_num_chars  = (buffer_size / sizeof(char));

	//+ create and start each thread
	if ( (threads = malloc(10000*sizeof(pthread_t))) == NULL )
		fail("malloc(num_threads) fail");
		
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	int period[3] = {10, 15, 35};
	int exeTime[3] = {2, 4, 10};
	int task_n = 3;
	int lcm_time = 1;
	int n_thread = 0;
	for (i = 0; i < task_n; i++)
		lcm_time = lcm_time / gcd(lcm_time, period[i]) * period[i];
	for (i = 0; i < lcm_time; i++) {
		// i-seconds
		for (j = 0; j < task_n; j++) {
			if (i % period[j] == 0) {
				targs = malloc(sizeof(*targs));
				targs->tid    = n_thread;
				targs->prio   = j;
				targs->mychar = (char) (j+START_CHAR);
				targs->nchars = (total_num_chars / num_threads);
				targs->exeTime = exeTime[j];
				printf("Time %2d create %c%d job exeTime = %d, job period = %d\n", 
						i, targs->mychar, targs->tid, exeTime[j], period[j]);
				syscall (SYS_weighted_rr_setquantum, exeTime[j]);
				syscall (SYS_RMS_setperiod, period[j]);
				pthread_create(&threads[n_thread], &attr, run, (void *)targs);
				n_thread++;

			}
		}
		sleep(1);
	}
	
	/*for (i = 0; i < num_threads; i++)
	{
		targs = malloc(sizeof(*targs));
		targs->tid    = i;
		targs->prio   = i;
		targs->mychar = (char) (i+START_CHAR);
		targs->nchars = (total_num_chars / num_threads);
		int exeTime = (rand()%20)+quantum;
		int period = (rand()%5)+exeTime;
		printf("%c job exeTime = %d, job period = %d\n",targs->mychar , exeTime, period);
		//if(quantum <= i)
		//	printf("TIme quantum too small\n");
		//else 
		syscall (SYS_weighted_rr_setquantum, exeTime);
		syscall (SYS_RMS_setperiod, period);
		pthread_create(&threads[i], &attr, run, (void *)targs);

		//if (sched_policy == SCHED_WEIGHTED_RR)quantum*=2;
	}*/

	//+ wait for all threads to complete
	for (i = 0; i < n_thread; i++) 
	{
		pthread_join(threads[i], NULL);
	}

	//+ print val_buf results
	int count = 1, cur_tid = tid_buf[0];
	for (i = 0, cur = val_buf[0]; i < total_num_chars; i++) 
	{
		if (cur != val_buf[i] || cur_tid != tid_buf[i]) 
		{
			if (round(1.f * count / 1000))
				printf("(%c%02d, %.1f)", cur, cur_tid, 1.f * count / 1000);
			cur = val_buf[i], cur_tid = tid_buf[i];
			count = 1;
		}
		else
		{
			count++;
		}
	}
	printf("(%c%2d, %.1f)\n", cur, cur_tid, 1.f * count / 1000);

	//+ print val_buf results
	printf("ignore eps\n");
	count = 1, cur_tid = tid_buf[0];
	for (i = 0, cur = val_buf[0]; i < total_num_chars; i++) 
	{
		if (cur != val_buf[i] || cur_tid != tid_buf[i]) 
		{
			if (round(1.f * count / 1000))
				printf("(%c%d, %.1f)", cur, cur_tid, 1.f * count / 1000);
			cur = val_buf[i], cur_tid = tid_buf[i];
			count = 1;
		}
		else
		{
			count++;
		}
	}
	printf("(%c%d, %d)\n", cur, cur_tid, count);
	printf("\n");
	
	//+ reset time quantum
	syscall (SYS_weighted_rr_setquantum, old_quantum);

	//+ clean up and exit
	pthread_attr_destroy(&attr);
	pthread_exit (NULL);
}
