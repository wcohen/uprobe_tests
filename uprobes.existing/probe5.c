#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>

#include "../include/udbgfs.c"
/*
 * usage:
 * $ make default
 * $ make progs
 * $ make add_addr
 * Make sure usp.vaddr in this file contains the address of add() in probeme4.
 * NOTE: You can now use this module to probe any address.  Just specify
 *	vaddr=0xwhatever on insmod.  It still defaults to add() in probeme4.
 * $ ./probeme4 5
 * I am process 3243.  Attach and press Enter:
 * # insmod probe5.ko pid=3243 [verbose=0]
 *	Turn off verbose if you specify a big number of iterations for
 *	probeme4.
 * Press Enter in probeme4's window.
 * # rmmod probe5	// First verify that unregister of a dead process works.
 * Check /var/log/messages.
 */

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int verbose = 1;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

static long vaddr = 0;
module_param(vaddr, long, 0);
MODULE_PARM_DESC(vaddr, "vaddr");

static int ncalls;

static void add_inst(struct uprobe *u, struct pt_regs *regs)
{
	if (verbose)
		test_printk("add() called\n");
	ncalls++;
}

struct uprobe usp = {
	.handler = add_inst,
	// <add> in ./probeme4
	.vaddr = 0x080484a8UL,
};

static void child_exit(void) 
{
	test_printk("ncalls=%d\n", ncalls);
}

int init_module(void)
{
	int ret;

	printk(KERN_INFO "In probe5 init_module \n");
	if (u_dbfs_init("probe5")) {
		printk(KERN_INFO "In probe5 u_dbfs_init failed\n");
		return -1;
	}
	usp.pid = pid;
	if (vaddr)
		usp.vaddr = vaddr;
	test_printk("Registering uprobe on pid %d, vaddr %#lx\n",
		usp.pid, usp.vaddr);
	ret = register_uprobe(&usp);
	if (ret != 0) {
		printk(KERN_INFO "register_uprobe() failed, returned %d\n",
				ret);
		u_dbfs_cleanup();
		return -1;
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "In probe5 cleanup_module \n");
	printk(KERN_INFO "Unregistering uprobe pid %d, vaddr %#lx\n",
		usp.pid, usp.vaddr);
	unregister_uprobe(&usp);

	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
