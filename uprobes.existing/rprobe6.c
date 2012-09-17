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
 * # insmod rprobe6.ko pid=3243 [verbose=0]
 *	Turn off verbose if you specify a big number of iterations for
 *	probeme4.
 * Press Enter in probeme6's window.
 * # rmmod rprobe6
 * Check /var/log/messages.
 * It should report that add() was called 5 times (since we specified
 * 5 iterations above) and bye(), adios(), etc. were each called once.
 * It should report that add() returned 5 times and bye(), adios(), etc.
 * did not return.
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
	int nret;
	int registered;
	struct uprobe u;
#ifdef ARCH_SUPPORTS_URETPROBES
	struct uretprobe rp;
#endif
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

#ifdef ARCH_SUPPORTS_URETPROBES
static void rp_handler(struct uretprobe_instance *ri, struct pt_regs *regs)
{
	struct probe_info *pi = container_of(ri->rp, struct probe_info, rp);
	pi->nret++;
	if (verbose)
		test_printk("%s returns %#lx\n", pi->fname, regs->eax);
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

#ifdef ARCH_SUPPORTS_URETPROBES
	pi->rp.u.pid = pid;
	pi->rp.u.vaddr = pi->u.vaddr;
	pi->rp.handler = rp_handler;
	ret = register_uretprobe(&pi->rp);
	if (ret != 0) {
		test_printk("register_uretprobe for %s returned %d\n",
			pi->fname, ret);
		unregister_uprobe(&pi->u);
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
#ifdef ARCH_SUPPORTS_URETPROBES
			unregister_uretprobe(&pi->rp);
#endif
		}
	}
}

static void child_exit(void) 
{
	struct probe_info *pi;
	for (pi = probes; pi->fname; pi++) {
		test_printk("rprobe6 %s=%d, calls=%d returns\n",
			pi->fname, pi->ncalls, pi->nret);
	}
}

static int __init rprobe6_init(void)
{
	struct probe_info *pi;
#ifdef ARCH_SUPPORTS_URETPROBES
#ifdef UPROBES_INIT_URETPROBES
	int result;
	unsigned long trampoline_addr;
#endif /*UPROBES_INIT_URETPROBES */
#endif /*ARCH_SUPPORTS_URETPROBES */

	printk(KERN_INFO "In rprobe6 init \n");
	if(u_dbfs_init("rprobe6")) {
		printk (KERN_INFO "In rprobe6 u_dbfs_init failed\n");
		return -1;
	}

#ifdef ARCH_SUPPORTS_URETPROBES
#ifdef UPROBES_INIT_URETPROBES

	trampoline_addr = find_vaddr("main");
	if (! trampoline_addr) {
		printk(KERN_INFO "Unknown function %s\n", "main");
		u_dbfs_cleanup();
		return -1;
	}
	result = init_uretprobes(pid, trampoline_addr);
	if (result != 0) {
		printk(KERN_INFO "init_uretprobes returned %d\n", result);
		u_dbfs_cleanup();
		return -1;
	}
#endif /*UPROBES_INIT_URETPROBES */
#endif
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

static void __exit rprobe6_exit(void)
{
	printk(KERN_INFO "In rprobe6 exit \n");
	unregister_all();

	u_dbfs_cleanup();
}

module_init(rprobe6_init);
module_exit(rprobe6_exit);
MODULE_LICENSE("GPL");
