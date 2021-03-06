#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>

#include "../include/udbgfs.c"
/*
 * Exercise [un]register_u[ret]probe from handlers.
 * An entry uprobe and a uretprobe are associated with the same probepoint.
 * The entry handler unregisters itself and registers the return handler.
 * The return handler unregisters itself and registers the entry handler.
 * The entry handler, which is registered first, and the return handler
 * should fire on alternate passes.
 *
 * usage:
 * $ make
 * NOTE: You can now use this module to probe any function.  Just specify
 *	vaddr=funcaddr tramp=trampaddr on insmod, where funcaddr is the
 *	probed function's address and tramp is the uretprobe trampoline
 *	address (e.g., main()'s address).  It still defaults to add() in
 *	probeme4.
 * $ ./probeme4 20
 * I am process 3243.  Attach and press Enter:
 * # insmod pong.ko pid=3243 [verbose=0]
 *	Turn off verbose if you specify a big number of iterations for
 *	probeme4.
 * Press Enter in probeme4's window.
 * # rmmod pong
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

static long tramp = 0;
module_param(tramp, long, 0);
MODULE_PARM_DESC(tramp, "tramp");

static int ncalls;
static int nreturns;
static int entry_probe_registered = 0;
static int return_probe_registered = 0;

#ifdef UPROBES_INIT_URETPROBES
static unsigned long trampoline_addr = 0x080484bc;
#endif

#ifdef UPROBES_REG_CALLBACK

static void reg_callback(struct uprobe *u, int reg, enum uprobe_type type,
	int result)
{
	if (!verbose)
		return;
	test_printk("%sregistration of %s on pid %d, vaddr %#lx was ",
		(reg ? "" : "un"),
		(type == UPTY_URETPROBE ? "uretprobe" : "uprobe"),
		u->pid, u->vaddr);
	if (result == 0)
		test_printk("successful\n");
	else
		test_printk("unsuccessful; status = %d\n", result);
}

#endif

struct uprobe entry_probe = {
	.vaddr = 0x080484a8UL,
#ifdef UPROBES_REG_CALLBACK
	.registration_callback = reg_callback
#endif
};
struct uretprobe return_probe = {
	.u.vaddr = 0x080484a8UL,
#ifdef UPROBES_REG_CALLBACK
	.registration_callback = reg_callback
#endif
};

static void entry_handler(struct uprobe *u, struct pt_regs *regs)
{
	int result;
	ncalls++;
	if (verbose)
		test_printk("ncalls = %d: Registering return handler\n",
			ncalls);
	result = register_uretprobe(&return_probe);
	if (result != -EINPROGRESS)
		test_printk("Entry handler's register_uretprobe() failed; returned %d\n", result);
	else
		return_probe_registered = 1;

	if (verbose)
		test_printk("Unregistering entry handler\n");
	unregister_uprobe(u);
	entry_probe_registered = 0;
}

static void return_handler(struct uretprobe_instance *ri, struct pt_regs *regs)
{
	int result;
	nreturns++;
	if (verbose)
		test_printk("returns = %d: Unregistering return handler\n",
			nreturns);
	unregister_uretprobe(ri->rp);
	return_probe_registered = 0;

	if (verbose)
		test_printk("Registering entry handler\n");
	result = register_uprobe(&entry_probe);
	if (result != -EINPROGRESS)
		test_printk("Return handler's register_uprobe() failed; returned %d\n", result);
	else
		entry_probe_registered = 1;
}

static void child_exit(void) 
{
	test_printk("ncalls=%d, nreturns=%d\n", ncalls, nreturns);
}

int init_module(void)
{
	int ret;

	printk(KERN_INFO "In pong init_module \n");
	if (u_dbfs_init("pong")) {
		printk(KERN_INFO "In pong u_dbfs_init failed\n");
		return -1;
	}

#ifdef UPROBES_INIT_URETPROBES
	if (tramp != 0)
		trampoline_addr = tramp;
	ret = init_uretprobes(pid, trampoline_addr);
	if (ret != 0) {
		printk(KERN_INFO
			"init_uretprobes(%d, %#lx) failed, returned %d\n",
			pid, trampoline_addr, ret);
		u_dbfs_cleanup();
		return -1;
	}
#endif
	entry_probe.pid = pid;
	entry_probe.handler = entry_handler;
	return_probe.u.pid = pid;
	return_probe.handler = return_handler;
	if (vaddr) {
		entry_probe.vaddr = vaddr;
		return_probe.u.vaddr = vaddr;
	}
	test_printk("Registering entry handler on pid %d, vaddr %#lx\n",
		entry_probe.pid, entry_probe.vaddr);
	ret = register_uprobe(&entry_probe);
	if (ret != 0) {
		printk(KERN_INFO "register_uprobe() failed, return=%d\n",
			       	ret);
		u_dbfs_cleanup();
		return -1;
	}
	entry_probe_registered = 1;
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "In pong cleanup_module \n");
	printk(KERN_INFO "Unregistering probes on pid %d, at vaddr %#lx\n",
		entry_probe.pid, entry_probe.vaddr);
	if (entry_probe_registered)
		unregister_uprobe(&entry_probe);
	if (return_probe_registered)
		unregister_uretprobe(&return_probe);

	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
