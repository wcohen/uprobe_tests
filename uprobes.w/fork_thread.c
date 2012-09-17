#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#include "sys/resource.h"
#include "sys/syscall.h"

// set debug to 1 for additional printfs
#define debug 0

#define UPROBES_UINFO 

#include "../include/u_test_common.h"

/* fork a child, run some probe points, wait for the child to exit,
 * count some more probes.
 */
uint fork_thread(int *iter)
{
	int status;
	pid_t pid;
	int cnt=*iter;
        int error=0;

	BUILD_PROBE_TABLE(probe_table,NUM_PROBES);
	if (debug) printf("Fork Thread started: tid=%d probe_table=%p\n",
				gettid(),probe_table);

	pid=fork();

 	if ( pid != 0 ){ /* parent */

#if 0
		kill(pid,SIGSTOP); /*  Wait here for the child */
		if(debug) printf("Parent contining\n");
#endif

		/* count some probe points hits */
		if(debug)printf("Pre- count\n");
		int i;
		for (i=0;i<cnt;++i){ 
			if ((error=probe_0(&probe_table[0]))<0) return(error);
		}

		/* wait for the child */
		wait(&status);

		/* run some more probes after the child exits. */
		if(debug)printf("Post-Count\n");
		for (i=0;i<cnt;++i){ /* parent probes */
			if ((error=probe_0(&probe_table[0]))<0) return(error);
		}
		
		if(WIFEXITED(status))
			if(debug)printf("Child exit status=%d\n",WEXITSTATUS(status));
			
		if(WEXITSTATUS(status)) return(WEXITSTATUS(status));

		/* everyone is happy */
		return(0); /** pass **/

	}else{ /* child */
		if (debug) printf("Child started: pid=%d tid=%d\n",
				 getpid(), gettid());

#if 0
		kill(pid,SIGCONT); /* Start the parent */
		if(debug) printf("Started parent pid=%d\n",pid);
#endif

		int i;
		for (i=0;i<cnt;++i){
                        if ((error=probe_0(&probe_table[0]))!=-1) exit(1);
		}
		exit(0); /*pass*/
	}
}


main(int argc, char **argv)
{
	int i,num_threads;
	pthread_t thread[MAX_THREADS];
	int niter;

	if ( (argc != 3 || (num_threads = atoi(argv[1])) <= 0 )
			 || (niter = atoi(argv[2])) <= 0) {
		fprintf(stderr,"Usage: %s number_of_threads num_iterations\n", argv[0]);
                exit(1);
        }

	/* stop here and wait for the uprobe mod to be loaded */
	wait_for_kthread();

	/* start fork threads */
	for(i=0;i<num_threads;i++){
		if(pthread_create(&thread[i],NULL,(void *)fork_thread,&niter)){
			perror("pthread_create failed");
			report_test_fail("pthread_create failed");
		}
	}
	
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
