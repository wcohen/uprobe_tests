#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>

#include "../include/udbgfs.c"
/*
 * Existence test for aggregate probes -- <nprobes> probes on probeme4:add()
 * usage:
 * $ make default
 * $ make progs
 * $ make add_addr
 * Make sure vaddr in this file contains the address of add() in probeme4.
 * $ ./probeme4 5
 * I am process 3243.  Attach and press Enter:
 * # insmod probe5ag.ko pid=3243 nprobes=3 [verbose=0]
 *	Turn off verbose if you specify a big number of iterations for
 *	probeme4.
 * Press Enter in probeme4's window.
 * # rmmod probe5ag
 * Check /var/log/messages.  Given the above example (5 iterations, 3 probes),
 * you should see:
 * "The handler was called a total of 15 times."
 */

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int nprobes = 6;
module_param(nprobes, int, 0);
MODULE_PARM_DESC(nprobes, "nprobes");

static int verbose = 1;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

static int vaddr = 0;
module_param(vaddr, int, 0);
MODULE_PARM_DESC(vaddr, "vaddr");

static int nhits;

static void add_inst(struct uprobe *u, struct pt_regs *regs)
{
	if (verbose)
		test_printk("add() called\n");
	nhits++;
}

struct uprobe *probes;


static void shut_down(void)
{
	int i;
	printk(KERN_INFO "Unregistering uprobes on probeme4:add()\n");
	for (i = 0; i < nprobes; i++) {
		if (probes[i].vaddr != 0)
			unregister_uprobe(&probes[i]);
	}
	kfree(probes);
}

static void child_exit(void) 
{
	test_printk("nhits=%d\n", nhits);
}

int init_module(void)
{
	int i, ret;
//	unsigned long vaddr = 0x080484a8U;

	printk(KERN_INFO "In probe5ag init_module \n");
	if (u_dbfs_init("probe5ag")) {
		printk(KERN_INFO "In probe5ag u_dbfs_init failed\n");
		return -1;
	}
	test_printk("Registering %d uprobes on pid %d, vaddr %#lx\n",
		nprobes, pid, vaddr);
	probes = (struct uprobe*) kzalloc(nprobes*sizeof(struct uprobe),
		GFP_KERNEL);
	if (!probes) {
		printk(KERN_INFO "No memory for probe array.\n");
		u_dbfs_cleanup();
		return -1;
	}
	for (i = 0; i < nprobes; i++) {
		probes[i].vaddr = vaddr;
		probes[i].pid = pid;
		probes[i].handler = add_inst;
		ret = register_uprobe(&probes[i]);
		if (ret != 0) {
			printk(KERN_INFO
				"register_uprobe() #%d failed, returned %d\n",
				i, ret);
			shut_down();
			u_dbfs_cleanup();
			return ret;
		}
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "In probe5ag cleanup_module \n");
	shut_down();

	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
