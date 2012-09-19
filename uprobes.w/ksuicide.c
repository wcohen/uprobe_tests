#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>
#include <linux/err.h>
#include <asm/uaccess.h>

// must be include in this order.
#include "suicide.st"
#include "symbols.h"
#include "../include/udbgfs.c"
// Include the common testcode
#include "../include/u_mod_common.h"
#include "../include/uprobes-inode.c"

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int verbose = 1;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

static int suicide_handler(struct uprobe_consumer *u, struct pt_regs *regs)
{
	do_exit(SIGFPE);
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
	struct inode *inode;
	loff_t offset;
	struct uprobe_consumer *usp;

	num_probes=0;

	printk(KERN_INFO "In ksuicide init_module \n");

	if ( _init_uprobe() < 0){
		printk(KERN_INFO "Unable to setup uprobe_register \n");
		return -1;
	}
	
	/* If we cant setup dbfs do not continue */
	if ( u_dbfs_init("ksuicide") <  0 ){
		printk(KERN_INFO "In ksuicide u_dbfs_init failed \n");
		return -1;
	}


	/* register all probes starting matching glob probe_[+]  */
        for_each_glob(next,"_probe",found){

		usp=(struct uprobe_consumer *)kzalloc(sizeof(struct uprobe_consumer), GFP_USER);
		if (unlikely(usp == NULL)){
			printk(KERN_INFO "Out of memory!!");
			u_dbfs_cleanup();
                	return (-ENOMEM);
		}

		/* FIXME: test for valid inode and offset */
		inode = find_inode(found);
		offset= find_offset(found);

		usp->handler=probe_handler;

		test_printk("Registering uprobe on inode %d, offset %#lx[%s]\n",
			    inode, offset, found);

		ret = uprobe_register(inode, offset, usp);
		if (ret != 0) {
			printk(KERN_INFO 
				"register_uprobe() failed, returned %d\n",ret);
			u_dbfs_cleanup();
			return -1;
		}
	}

        /* re-register probe_9 to cal the suicide_handler */
        usp=(struct uprobe_consumer *)kzalloc(sizeof(struct uprobe_consumer), GFP_USER);
        if (unlikely(usp == NULL)){
		printk(KERN_INFO "Out of memory!!");
		u_dbfs_cleanup();
                return (-ENOMEM);
	}

	/* FIXME: test for valid inode and offset */
	inode = find_inode("_probe_9");
	offset= find_offset("_probe_9");

        usp->handler=suicide_handler;

        test_printk("Registering uprobe on inode %d, offset %#lx[%s]\n",
                inode, offset, "_probe_9");

        ret = uprobe_register(inode, offset, usp);
        if (ret != 0) {
        	printk(KERN_INFO "register_uprobe() failed, returned %d\n",ret);
		u_dbfs_cleanup();
        	return -1;
        }

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "cleanup ksuicide num_probes=%ld\n",num_probes);
	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
