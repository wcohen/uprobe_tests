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
	
uint thread_1()
{
	int error=0;

	BUILD_PROBE_TABLE(probe_table,NUM_PROBES);
	if (debug) printf("Thread started: tid=%d probe_table=%p\n",gettid(),probe_table);

	if ((error=probe_0(&probe_table[0]))<0) return(error);
	if ((error=probe_1(&probe_table[1]))<0) return(error);
	if ((error=probe_2(&probe_table[2]))<0) return(error);
	if ((error=probe_3(&probe_table[3]))<0) return(error);
	if ((error=probe_4(&probe_table[4]))<0) return(error);
	if ((error=probe_5(&probe_table[5]))<0) return(error);
	if ((error=probe_6(&probe_table[6]))<0) return(error);
	if ((error=probe_7(&probe_table[7]))<0) return(error);
	if ((error=probe_8(&probe_table[8]))<0) return(error);
	if ((error=probe_9(&probe_table[9]))<0) return(error);
	return(0); /* pass */
}

uint thread_2()
{
	/* this thread is run with no probe points registered */
	int error;

	BUILD_PROBE_TABLE(probe_table,NUM_PROBES);
	if (debug) printf("Thread started: tid=%d probe_table=%p\n",gettid(),probe_table);

	if ((error=probe_0(&probe_table[0]))!= -1) return(error);
	if ((error=probe_1(&probe_table[1]))!= -1) return(error);
	if ((error=probe_2(&probe_table[2]))!= -1) return(error);
	if ((error=probe_3(&probe_table[3]))!= -1) return(error);
	if ((error=probe_4(&probe_table[4]))!= -1) return(error);
	if ((error=probe_5(&probe_table[5]))!= -1) return(error);
	if ((error=probe_6(&probe_table[6]))!= -1) return(error);
	if ((error=probe_7(&probe_table[7]))!= -1) return(error);
	if ((error=probe_8(&probe_table[8]))!= -1) return(error);
	if ((error=probe_9(&probe_table[9]))!= -1) return(error);
	return(0); /* pass */
}


/*The uprobe module will place a probe on this function.  Calling this
 *function will cause the uprobes module to register all the probes.  
 */
void reg_all(void)
{
	if (debug) printf("Registering all probe points\n");
}

/*The uprobe module will place a probe on this function.  Calling this
 *function will cause the uprobes module to un-register all the probes. 
 */
void unreg_all(void)
{
	if (debug) printf("Un-registering all probe points\n");
}

main(int argc, char **argv)
{
	int i,num_threads,niter,results;
	pthread_t thread[MAX_THREADS];

	if (argc != 3 || (num_threads = atoi(argv[1])) <= 0 
			|| (niter = atoi(argv[2])) <= 0) {
	   fprintf(stderr, "Usage: %s number_of_threads num_iterations\n", argv[0]);
	   exit(1);
	}

	wait_for_kthread(); /* stop here and wait for the uprobe mod to be loaded */

	int j;
        for (j=0;j<niter;++j){
		/* register probe points */
		reg_all();

		/* start thread_1's */
		for(i=0;i<num_threads;i++){
			if (pthread_create(&thread[i], NULL, (void *)thread_1,&niter)){
				perror("pthread_create failed");
				report_test_fail("pthread_create failed");
			}
		}
		if (debug ) printf("%d threads started\n",i);

        	/* Gather around */
        	for(i=0;i<num_threads;i++){
                	if (pthread_join(thread[i],(void **)&results) != 0)
                        	report_test_fail("pthread_join returned an error\n");
                	if ( results != 0 ) break;
        	}
		/* check if all threads saw the probe points */
        	if(debug) printf("%d threads returned\n",i);
        	if ((i != num_threads)||( results != 0 )) report_fail();

		/* un-register probe points */
		unreg_all();

		/* start thread_2's */
		for(i=0;i<num_threads;i++){
			if (pthread_create(&thread[i], NULL, (void *)thread_2,&niter)){
				perror("pthread_create failed");
				report_test_fail("pthread_create failed");
			}
		}
                if (debug ) printf("%d threads started\n",i);

		/* Gather around */
		for(i=0;i<num_threads;i++){
		if (pthread_join(thread[i],(void **)&results) != 0)
			report_test_fail("pthread_join returned an error\n");
			if ( results != 0 ) break;
		}
		/* check if all threads ran without seeing any probe points hit*/ 
		if(debug) printf("%d threads returned\n",i);
		if ((i != num_threads)||( results != 0 )) report_fail();
	} /* for */
	report_pass();
}
