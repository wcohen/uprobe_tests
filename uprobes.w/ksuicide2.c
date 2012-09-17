#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/utrace.h>
#include <linux/uprobes.h>
#include <linux/err.h>
#include <asm/uaccess.h>

// must be include in this order.
#include "suicide.st"
#include "symbols.h"
#include "../include/udbgfs.c"
// Include the common testcode
#include "../include/u_mod_common.h"

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int verbose = 1;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

static void suicide_handler(struct uprobe *u, struct pt_regs *regs)
{
        char *a;
        a=0;
        a[1]='A';
}

static void child_exit(void) 
{
	test_printk("num_probes=%ld\n", num_probes);
}

int init_module(void)
{
	int ret;
	int next;
	char *found;
	struct uprobe *usp;

	num_probes=0;

	printk(KERN_INFO "In ksuicide2 init_module \n");
	/* If we cant setup dbfs do not continue */
	if ( u_dbfs_init("ksuicide2") <  0 ){
		printk(KERN_INFO "In ksuicide2 u_dbfs_init failed \n");
		return -1;
	}


	/* register all probes starting matching glob probe_[+]  */
        for_each_glob(next,"_probe",found){

		usp=(struct uprobe *)kzalloc(sizeof(struct uprobe), GFP_USER);
		if (unlikely(usp == NULL)){
			printk(KERN_INFO "Out of Memory\n");
			u_dbfs_cleanup();
                	return (-ENOMEM);
		}

		usp->pid=pid;
		/* FIXME: test for valid address */
		usp->vaddr=find_vaddr(found); 

		usp->handler=probe_handler;

		test_printk("Registering uprobe on pid %d, vaddr %#lx[%s]\n",
			usp->pid, usp->vaddr,found);

		ret = register_uprobe(usp);
		if (ret != 0) {
			printk(KERN_INFO 
				"register_uprobe() failed, returned %d\n",ret);
			u_dbfs_cleanup();
			return -1;
		}
	}

        /* re-register probe_9 to cal the suicide_handler */
        usp=(struct uprobe *)kzalloc(sizeof(struct uprobe), GFP_USER);
        if (unlikely(usp == NULL)){
		printk(KERN_INFO "Out of Memory\n");
		u_dbfs_cleanup();
                return (-ENOMEM);
	}

        usp->pid=pid;
        usp->vaddr=find_vaddr("_probe_9");

        usp->vaddr=usp->vaddr;
        usp->handler=suicide_handler;

        test_printk("Registering uprobe on pid %d, vaddr %#lx[%s]\n",
                usp->pid, usp->vaddr,"_probe_9");

        ret = register_uprobe(usp);
        if (ret != 0) {
        	printk(KERN_ERR "register_uprobe() failed, returned %d\n",ret);
		u_dbfs_cleanup();
        	return -1;
        }

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "cleanup ksuicide2 num_probes=%ld\n", num_probes); 
	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
