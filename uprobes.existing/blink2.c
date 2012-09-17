#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>

#include "probeme4s.st"
#include "../include/udbgfs.c"

/*
 * A variation of blink.c: the two probes are on different probepoints.
 * One handler, steady(), registers the other, blink(), whenever the
 * ncalls count is a multiple of 3 (and blink isn't already registered).
 * blink unregisters itself every time ncalls is a multiple of 5.
 *
 * usage:
 * $ make default
 * $ ./probeme4s 50
 * I am process 3243.  Attach and press Enter:
 * # insmod blink2.ko pid=3243 [verbose=0]
 *	Turn off verbose if you specify a big number of iterations for
 *	probeme4s.
 * Press Enter in probeme4s's window.
 * # rmmod blink2
 * Check /var/log/messages.
 */

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int verbose = 1;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

static int ncalls;
static int blink_registered = 0;

#ifdef UPROBES_REG_CALLBACK

static void reg_callback(struct uprobe *u, int reg, enum uprobe_type type,
	int result)
{
	if (!verbose)
		return;
	test_printk("%sregistration of uprobe on pid %d, vaddr %#lx was ",
		(reg ? "" : "un"), u->pid, u->vaddr);
	if (result == 0)
		test_printk("successful\n");
	else
		test_printk("unsuccessful; status = %d\n", result);
}

#endif

static void blink(struct uprobe *u, struct pt_regs *regs)
{
	if (ncalls % 5 == 0) {
		if (verbose)
			test_printk(
				"ncalls = %d: Unregistering blink handler\n",
				ncalls);
		unregister_uprobe(u);
		blink_registered = 0;
	}
}

struct uprobe usp_blink = {
	.handler = blink,
#ifdef UPROBES_REG_CALLBACK
	.registration_callback = reg_callback
#endif
};

static void steady(struct uprobe *u, struct pt_regs *regs)
{

#ifdef UPROBES_REG_CALLBACK
	int result;
#endif
	ncalls++;
	if (!blink_registered && ncalls % 3 == 0) {
		if (verbose)
			test_printk(
				"ncalls = %d: Registering blink handler\n",
				ncalls);

#ifdef UPROBES_REG_CALLBACK
		result = register_uprobe(&usp_blink);
		if (result != -EINPROGRESS)
			test_printk(
			"steady handler's register_uprobe() failed; returned %d\n",
			result);
		else
			blink_registered = 1;
#endif
	}
}

struct uprobe usp_steady = {
	.handler = steady,
#ifdef UPROBES_REG_CALLBACK
	.registration_callback = reg_callback
#endif
};

static void child_exit(void)
{
	test_printk("ncalls=%d\n", ncalls);
}

int init_module(void)
{
	int ret;

	printk(KERN_INFO "In blink2 init_module \n");
	if (u_dbfs_init("blink2")) {
		printk(KERN_INFO "In blink2 u_dbfs_init failed \n");
		return -1;
	}
	usp_steady.pid = pid;
	usp_steady.vaddr = find_vaddr("add");
	if (!usp_steady.vaddr) {
		printk(KERN_INFO "Unknown function add\n");
		u_dbfs_cleanup();
		return -1;
	}
	usp_blink.pid = pid;
	usp_blink.vaddr = find_vaddr("subtract");
	if (!usp_blink.vaddr) {
		printk(KERN_INFO "Unknown function subtract\n");
		u_dbfs_cleanup();
		return -1;
	}
	test_printk("Registering steady handler on pid %d, vaddr %#lx\n",
		usp_steady.pid, usp_steady.vaddr);
	ret = register_uprobe(&usp_steady);
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
	printk(KERN_INFO "In blink2 cleanup_module\n");
	printk(KERN_INFO "Unregistering uprobe[s] on pid %d\n",
		usp_steady.pid);
	unregister_uprobe(&usp_steady);
	if (blink_registered)
		unregister_uprobe(&usp_blink);

	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
