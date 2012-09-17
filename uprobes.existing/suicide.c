#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>
#include <asm/signal.h>

#include "probeme6.st"
#include "../include/udbgfs.c"

/*
 * usage:
 * $ make progs
 * $ make default
 * $ ./probeme6 5	# Specify any positive integer.
 * I am process 3243.  Attach and press Enter:
 * # insmod suicide.ko pid=3243
 * Press Enter in probeme6's window.
 * probeme6 should die immediately with a SIGFPE.
 * Check /var/log/messages.
 * It should report that task 3243 died in the the uprobe handler.
 * # rmmod suicide
 * rmmod should work.
 */

static int pid = 0;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

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
	{ NULL }
};

static void u_handler(struct uprobe *u, struct pt_regs *regs)
{
	do_exit(SIGFPE);
}

static void child_exit (void) 
{
	printk("In child_exit suicide module\n");
}

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

static int __init suicide_init(void)
{
	struct probe_info *pi;
	printk(KERN_INFO "In suicide init \n");
	if (u_dbfs_init("suicide")) {
		printk("In suicide u_dbfs_init failed\n");
		return -1;
	}
	for (pi = probes; pi->fname; pi++)
		pi->registered = 0;

	for (pi = probes; pi->fname; pi++) {
		test_printk("Registering probes for %s\n", pi->fname);
		printk("Registering probes for %s\n", pi->fname);
		if (register_probes(pi) != 0) {
			printk(KERN_INFO "registering of probes failed\n");
			unregister_all();
			u_dbfs_cleanup();
			return -1;
		}
	}
	return 0;
}

static void __exit suicide_exit(void)
{
	printk(KERN_INFO "suicide test cleanup\n");
	unregister_all();
	u_dbfs_cleanup();
}

module_init(suicide_init);
module_exit(suicide_exit);
MODULE_LICENSE("GPL");
