#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// set debug to 1 for additional printfs
#define debug 0

#include "../include/u_test_common.h"

/*
 * Test uprobes for (no) probe inheritance across a fork()
 *
 * Uses the probe5.ko kernel module
 *
 * This program's output:
 *
 * [ananth@xyz test]$ ./probe8-fork 10
 * I am process 4868.  Attach and press Enter:
 * In child! My pid = 4871
 * 10 interations in 0.000153 sec
 * sum = 55
 * Parent exiting!
 * 10 interations in 0.000001 sec
 * sum = 55
 * Child exiting!
 *
 * dmesg output:
 * [root@xyz test]# dmesg
 * Registering uprobe on pid 4868, vaddr 0x8048574
 * Unregistering uprobe on probeme4:add()
 * ncalls = 10
 */
noinline fastcall static int add(int n1, int n2)
{
	return n1 + n2;
}

void calc_sum(int niter)
{
	int i, sum;
	struct timeval start, finish;
	long usec;

	sum = 0;
	(void) gettimeofday(&start, NULL);
	for (i = 1; i <= niter; i++) {
		sum = add(sum, i);
	}
	(void) gettimeofday(&finish, NULL);
	usec = 1000*1000*(finish.tv_sec - start.tv_sec)
		+ (finish.tv_usec - start.tv_usec);
	printf("%d interations in %.6f sec\n",
		       niter, ((double)usec) / (1000*1000));
	printf("sum = %d\n", sum);
	return;
}

int main(int argc, char **argv)
{
	int n;
	pid_t pid;
	char line[100];
	int status=-1;

	if (argc != 2 || (n = atoi(argv[1])) <= 0) {
		sprintf(line, "Usage: %s niter\n", argv[0]);
		report_test_fail(line);
	}

	//fprintf(stderr, "I am process %d.  Attach and press Enter:", getpid());
	//fgets(line, 100, stdin);

	kill(getpid(),SIGSTOP);

	pid = fork();
	if (pid < 0)
		report_test_fail("fork!\n");

	switch (pid) {
	case 0:
		//fprintf(stderr, "In child! My pid = %d\n", getpid());
		calc_sum(n);
		_exit(0);
		break;
	default:
		calc_sum(n);
		break;
	}
	wait(&status);
	if (status == 0 )
		report_pass();
	else
		report_test_fail("Child exited with error");
}
