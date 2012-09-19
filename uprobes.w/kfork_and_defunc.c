#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>
#include <linux/err.h>
#include <asm/uaccess.h>

// must be include in this order.
#include "fork_and_defunc.st"
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

	printk(KERN_INFO "In kfork_and_defunc  init_module \n");

	if ( _init_uprobe() < 0){
		printk(KERN_INFO "Unable to setup uprobe_register \n");
		return -1;
	}
	
	/* If we cant setup dbfs do not continue */
	if ( u_dbfs_init("kfork_and_defunc") <  0 ){
		printk(KERN_INFO "In fork_and_defunc u_dbfs_init failed \n");
		return -1;
	}


	/* register all probes starting matching glob probe_[+]  */
        for_each_glob(next,"_probe",found){

		usp=(struct uprobe_consumer *)kzalloc(sizeof(struct uprobe_consumer), GFP_USER);
		if (unlikely(usp == NULL)){
			printk(KERN_INFO 
				"register_uprobe() failed, returned %d\n", 
				ret);
			u_dbfs_cleanup();
                	return (-ENOMEM);
		}

		/* FIXME: test for valid inode and offset */
		inode = find_inode(found);
		offset= find_offset(found);

		test_printk("Registering uprobe on inode %d, offset %#lx[%s]\n",
			    inode, offset, found);

		ret = uprobe_register(inode, offset, usp);
		if (ret != 0) {
			printk(KERN_INFO 
				"register_uprobe() failed, returned %d\n", 
				ret);
			u_dbfs_cleanup();
			return -1;
		}
	}
	return 0;
}

void cleanup_module(void)
{
	printk("cleanup kfork_and_defunc num_probes=%ld\n", num_probes);
	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
