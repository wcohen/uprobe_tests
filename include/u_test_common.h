
#define noinline __attribute__((noinline))
#define fastcall __attribute__((regparm(3)))

#ifdef UPROBES_UINFO  

#define NUM_PROBES 10
#define MAX_THREADS 1000

struct uinfo {
	pid_t utid,ktid;
	ulong ucount, kcount;
	int results;
};

// create gettid call *!@#
static pid_t gettid (void){return syscall(__NR_gettid);}



// This defines the probe_#() functions. Uprobe breakpoints are
// set on the function address.					
// returns: -1=kernel is not counting
//	    -2= user=!kernel
//	    kcount 
// FIXME handle counter wrap

void *findme;

#define PROBE_FUN(id)						\
 		noinline int _probe_##id(void *ptr) {		\
		ulong ucount, kcount;				\
		struct uinfo *slot = (struct uinfo *)ptr;	\
								\
		slot->ucount++;					\
		ucount=slot->ucount;				\
		kcount=slot->kcount;				\
								\
		if (debug){					\
			int _id = id;				\
			printf("debug: probe_%d utid=%d ktid=%d ptr=%p",\
				_id,slot->utid, slot->utid,ptr);	\
                	printf(" kcount=%ld ucount=%ld\n",kcount,ucount);\
		}						\
		if ((kcount == -1)||(kcount==ucount))		\
			return kcount;				\
		if (debug){					\
			printf("ERROR: counts do not match:");	\
			printf("probe_%d utid=%d ktid=%d slot=%p ",\
				id, slot->utid,slot);		\
                	printf("kcount=%ld ucount=%ld\n",kcount,ucount);\
		}						\
		return -2;					\
	}							\
	static noinline int probe_##id(void *ptr) {		\
		struct uinfo *slot = (struct uinfo *)ptr;	\
		if(slot->utid){					\
			if( slot->utid != gettid() )		\
			   report_test_fail("found wrong tid");	\
		}else{						\
			slot->utid = gettid();			\
		}						\
		return _probe_##id(ptr);			\
	}

/* create the probe point functions probe_0 --> probe_9 */
PROBE_FUN(0);
PROBE_FUN(1);
PROBE_FUN(2);
PROBE_FUN(3);
PROBE_FUN(4);
PROBE_FUN(5);
PROBE_FUN(6);
PROBE_FUN(7);
PROBE_FUN(8);
PROBE_FUN(9);


#define	BUILD_PROBE_TABLE(table_name,size)		\
	struct uinfo *table_name=calloc(size,sizeof(struct uinfo));\
	int _j;						\
	for(_j=0;_j<size;_j++){				\
		table_name[_j].kcount=-1;		\
		table_name[_j].ucount=-1;		\
		table_name[_j].utid = (pid_t)0;		\
		table_name[_j].ktid = (pid_t)0;		\
	}

#endif /* UPROBES_UINFO */



int wait_for_kthread(void)
{
        kill(getpid(),SIGSTOP);
        return 0;
}



int report_pass(void)
{
	printf("***PASSED***\n\n");
	exit(0);
}

int report_fail(void)
{
	printf("!!!FAILED!!!\n\n");
	exit(1);
}

int report_test_fail(char *msg)
{
	printf("!!!FAILED!!! - Internal test failure - %s\n\n",msg);
	exit(1);
}
