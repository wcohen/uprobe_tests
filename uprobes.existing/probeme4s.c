#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// set debug to 1 for additional printfs
#define debug 0

#include "../include/u_test_common.h"

noinline fastcall static int add(int n1, int n2)
{
	return n1 + n2;
}

noinline fastcall static subtract(int n1, int n2)
{
	return n1 - n2;
}

main(int argc, char **argv)
{
	int i, n, sum;
	struct timeval start, finish;
	long usec;
	char line[100];

	if (argc != 2 || (n = atoi(argv[1])) <= 0) {
		sprintf(line, "Usage: %s niter\n", argv[0]);
		report_test_fail(line);
	}

	//fprintf(stderr, "I am process %d.  Attach and press Enter:", getpid());
	//fgets(line, 100, stdin);

	kill(getpid(),SIGSTOP);
	printf("continuing \n");
	sum = 0;
	(void) gettimeofday(&start, NULL);
	for (i = 1; i <= n; i++) {
		sum = add(sum, i);
		sum = subtract(sum, i);
	}
	(void) gettimeofday(&finish, NULL);
	usec = 1000*1000*(finish.tv_sec - start.tv_sec)
		+ (finish.tv_usec - start.tv_usec);
	printf("%d interations in %.6f sec\n", n, ((double)usec) / (1000*1000));
	printf("%.6f usec per iteration\n", ((double)usec) / n);
	printf("sum = %d\n", sum);
	report_pass();
}
