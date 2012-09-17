#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <errno.h>
#include <signal.h>

#include "sys/resource.h"
#include "sys/syscall.h"

// set debug to 1 for additional printfs
#define debug 0

#define UPROBES_UINFO 

#include "../include/u_test_common.h"

uint thread_1(int *iter)
{
	int cnt=*iter;
	int error=0;

	BUILD_PROBE_TABLE(probe_table,NUM_PROBES);
	if (debug) printf("Thread started: tid=%d probe_table=%p\n",gettid(),probe_table);

	int i;
	for (i=0;i<cnt;++i){
		if ((error=probe_0(&probe_table[0]))<0) break;
		if ((error=probe_1(&probe_table[1]))<0) break;
		if ((error=probe_2(&probe_table[2]))<0) break;
		if ((error=probe_3(&probe_table[3]))<0) break;
		sleep(1); /* force a reschedule */
		if ((error=probe_4(&probe_table[4]))<0) break;
		if ((error=probe_5(&probe_table[5]))<0) break;
		if ((error=probe_6(&probe_table[6]))<0) break;
		if ((error=probe_7(&probe_table[7]))<0) break;
		if ((error=probe_8(&probe_table[8]))<0) break;
	}
	if( error >= 0 ) return(0); /* pass */
	return(error); /* fail */
}

uint suicide_thread(int *iter)
{
        int cnt=*iter;
        int error=0;

        BUILD_PROBE_TABLE(probe_table,NUM_PROBES);
        if (debug) printf("Thread started: tid=%d probe_table=%p\n",gettid(),probe_table);

	/* this is the probe point that will die in the handler */
       probe_9(&probe_table[9]);
       return(0); /* pass */
}


main(int argc, char **argv)
{
	int i,num_threads,niter;
	pthread_t thread[MAX_THREADS];

	if (argc != 3 || (num_threads = atoi(argv[1])) <= 0 
					|| (niter = atoi(argv[2])) <= 0) {
		fprintf(stderr,"Usage: %s number_of_threads num_iterations\n", argv[0]);
		exit(1);
	}

	/* Stop here and wait for the uprobe mod to be loaded */
	wait_for_kthread(); 

	/* start some thread_1's */
	for(i=0;i<num_threads-1;i++){
		if (pthread_create(&thread[i], NULL, (void *)thread_1,&niter)){
			perror("pthread_create failed");
			report_test_fail("pthread_create failed");
		}
	}
	if (debug ) printf("%d threads started\n",i);

	/* Start the suicide_thread */
	if (pthread_create(&thread[i], NULL, (void *)suicide_thread,&niter)){
		perror("pthread_create failed");
		report_test_fail("pthread_create failed");
	}
	if (debug ) printf("suicide_thread started\n",i);

	
        /* Gather around */
        int results;
        for(i=0;i<num_threads;i++){
                if (pthread_join(thread[i],(void **)&results) != 0)
                        report_test_fail("pthread_join returned an error\n");
                if ( results != 0 ) break;
        }
        if(debug) printf("%d threads returned\n",i);

        if ((i==num_threads)&&( results == 0 )) report_pass();
        report_fail();
}
