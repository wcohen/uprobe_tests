#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>

#include "probeme6.st"
#include "../include/udbgfs.c"

/*
 * usage:
 * $ make progs
 * $ make default
 * $ ./probeme6 5
 * I am process 3243.  Attach and press Enter:
 * # insmod probe6.ko pid=3243 [verbose=0]
 *	Turn off verbose if you specify a big number of iterations for
 *	probeme4.
 * Press Enter in probeme6's window.
 * # rmmod probe6
 * Check /var/log/messages.
 * It should report that add() was called 5 times (since we specified
 * 5 iterations above) and bye(), adios(), etc. were each called once.
 */

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int verbose = 1;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

struct probe_info {
	const char *fname;
	int ncalls;
	// int nret;
	int registered;
	struct uprobe u;
	// struct kretprobe rp;
};

struct probe_info probes[] = {
	{ "add" },
	{ "bye" },
	{ "adios" },
	{ "au_revoir" },
	{ "auf_wiedersehen" },
	{ "arrivederci" },
	{ NULL }
};

static void u_handler(struct uprobe *u, struct pt_regs *regs)
{
	struct probe_info *pi = container_of(u, struct probe_info, u);
	pi->ncalls++;
	if (verbose)
		test_printk("%s called\n", pi->fname);
}

#if 0
static int rp_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	struct probe_info *pi = container_of(ri->rp, struct probe_info, rp);
	pi->nret++;
	test_printk("%s returns %#lx\n", pi->fname, regs->eax);
	return 0;
}
#endif

static int register_probes(struct probe_info *pi)
{
	int ret;

	pi->u.pid = pid;
	pi->u.vaddr = find_vaddr(pi->fname);
	if (! pi->u.vaddr) {
		test_printk("Unknown function %s\n", pi->fname);
		return -1;
	}

	pi->u.handler = u_handler;
	ret = register_uprobe(&pi->u);
	if (ret != 0) {
		test_printk("register_uprobe for %s returned %d\n",
			pi->fname, ret);
		return -1;
	}

#if 0
	pi->rp.kp.addr = pi->kp.addr;
	pi->rp.handler = rp_handler;
	pi->rp.maxactive = 100;
	ret = register_kretprobe(&pi->rp);
	if (ret != 0) {
		test_printk("register_kretprobe for %s returned %d\n",
			pi->fname, ret);
		unregister_kprobe(&pi->kp);
		return -1;
	}
#endif
	pi->registered = 1;
	return 0;
}

static void unregister_all(void)
{
	struct probe_info *pi;
	for (pi = probes; pi->fname; pi++) {
		if (pi->registered) {
			unregister_uprobe(&pi->u);
			// unregister_kretprobe(&pi->rp);
		}
	}
}

static void child_exit(void) 
{
	struct probe_info *pi;
	for (pi = probes; pi->fname; pi++) {
		test_printk("probe6 %s=%d calls\n", pi->fname,
				pi->ncalls);
		// test_printk("%s: %d calls, %d returns, %d missed\n",
		//	pi->fname, pi->ncalls, pi->nret, pi->rp.nmissed);
	}
}

static int __init probe6_init(void)
{
	struct probe_info *pi;

	printk(KERN_INFO "In probe6 init \n");
	if (u_dbfs_init("probe6")) {
		printk(KERN_INFO "In probe6 u_dbfs_init failed\n");
		return -1;
	}
	for (pi = probes; pi->fname; pi++)
		pi->registered = 0;

	for (pi = probes; pi->fname; pi++) {
		test_printk("Registering probes for %s\n", pi->fname);
		if (register_probes(pi) != 0) {
			printk(KERN_INFO "Registering probes failed\n");
			unregister_all();
			u_dbfs_cleanup();
			return -1;
		}
	}
	return 0;
}

static void __exit probe6_exit(void)
{
	printk(KERN_INFO "In probe6 module exit\n");
	unregister_all();

	u_dbfs_cleanup();
}

module_init(probe6_init);
module_exit(probe6_exit);
MODULE_LICENSE("GPL");
