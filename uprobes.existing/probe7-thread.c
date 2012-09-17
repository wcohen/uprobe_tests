#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

// set debug to 1 for additional printfs
#define debug 0

#include "../include/u_test_common.h"


/*
 * Testing uprobes in multithreaded scenarios
 *
 * Test this in conjunction with the probe5 kernel module
 *
 * This program's output:
 * [ananth@xyz test]$ ./probe7-thread 100
 * I am process 3965.  Attach and press Enter:
 * I am thread1!
 * thread1: 100 interations in 0.001408 sec
 * thread1: 14.080000 usec per iteration
 * thread1 sum = 5050
 * thread1: exiting
 * I am thread2!
 * thread2: 100 interations in 0.001405 sec
 * thread2: 14.050000 usec per iteration
 * thread2 sum = 5050
 * thread2: exiting
 * Parent exiting
 *
 * dmesg:
 * [root@xyz test]# dmesg
 * Registering uprobe on pid 3965, vaddr 0x80485d4
 * Unregistering uprobe on probeme4:add()
 * ncalls = 200
 */

unsigned int niter;

noinline fastcall static int add(int n1, int n2)
{
	return n1 + n2;
}

void *start_thread1(void *ptr)
{
	int i, sum;
	struct timeval start, finish;
	long usec;

	fprintf(stderr, "I am thread1!\n");
	sum = 0;
	(void) gettimeofday(&start, NULL);
	for (i = 1; i <= niter; i++) {
		sum = add(sum, i);
	}
	(void) gettimeofday(&finish, NULL);
	usec = 1000*1000*(finish.tv_sec - start.tv_sec)
		+ (finish.tv_usec - start.tv_usec);
	printf("thread1: %d interations in %.6f sec\n",
		       niter, ((double)usec) / (1000*1000));
	printf("thread1: %.6f usec per iteration\n", ((double)usec) / niter);
	printf("thread1 sum = %d\n", sum);
	fprintf(stderr, "thread1: exiting\n");
	return NULL;
}

void *start_thread2(void *ptr)
{
	int i, sum;
	struct timeval start, finish;
	long usec;

	fprintf(stderr, "I am thread2!\n");
	sum = 0;
	(void) gettimeofday(&start, NULL);
	for (i = 1; i <= niter; i++) {
		sum = add(sum, i);
	}
	(void) gettimeofday(&finish, NULL);
	usec = 1000*1000*(finish.tv_sec - start.tv_sec)
		+ (finish.tv_usec - start.tv_usec);
	printf("thread2: %d interations in %.6f sec\n",
		       niter, ((double)usec) / (1000*1000));
	printf("thread2: %.6f usec per iteration\n", ((double)usec) / niter);
	printf("thread2 sum = %d\n", sum);
	fprintf(stderr, "thread2: exiting\n");
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t thread1, thread2;
	int ret1, ret2;
	char line[100], *tmesg1 = "Thread 1 here", *tmesg2 = "Thread 2 here";

	if (argc != 2 || (niter = atoi(argv[1])) <= 0) {
		sprintf(line, "Usage: %s niter\n", argv[0]);
		report_test_fail(line);
	}

//	fprintf(stderr, "I am process %d.  Attach and press Enter:", getpid());
//	fgets(line, 100, stdin);

	kill(getpid(),SIGSTOP);

	ret1 = pthread_create(&thread1, NULL, start_thread1, (void *)tmesg1);
	ret2 = pthread_create(&thread2, NULL, start_thread2, (void *)tmesg2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	report_pass();
	return 0; // Redundunt return just to avoid a warning message.
}
