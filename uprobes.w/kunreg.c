#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/utrace.h>
#include <linux/uprobes.h>
#include <linux/err.h>
#include <asm/uaccess.h>

// must be include in this order.
#include "unreg.st"
#include "symbols.h"
#include "../include/udbgfs.c"
// Include the common testcode
#include "../include/u_mod_common.h"

#define MAX_PROBES 100
int num_reg_probes;
struct uprobe *probes[MAX_PROBES];
static int pid = 0;

module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "pid");

static int verbose = 0;
module_param(verbose, int, 0);
MODULE_PARM_DESC(verbose, "verbose");

static void unregister_all(struct uprobe *u, struct pt_regs *regs)
{
	int i;
	printk(KERN_INFO "Unregistering %d probes\n",num_reg_probes);
	for (i=0 ;i<=num_reg_probes ;i++){
		unregister_uprobe(probes[i]);
	}
}


static void register_all(struct uprobe *u, struct pt_regs *regs)
{
        int ret;
        int next;
        char *found;
        struct uprobe *usp;

        num_reg_probes=0;

        /* register all probes starting matching glob probe_[+]  */
        for_each_glob(next,"_probe",found){
                usp=(struct uprobe *)kzalloc(sizeof(struct uprobe),GFP_USER);
                if (unlikely(usp == NULL)){
			test_printk("kzalloc() failed, returned NULL");
			return;
		}

                usp->pid=pid;
                usp->vaddr=find_vaddr(found);
                usp->handler=probe_handler;

                test_printk("Registering uprobe on pid %d, vaddr %#lx[%s]\n",
                        usp->pid, usp->vaddr,found);

                ret = register_uprobe(usp);
                if ((ret != 0) && (ret != -EINPROGRESS)) {
                        test_printk("register_uprobe() failed, returned %d\n", ret);
			return;
		}

		test_printk("OK\n");

                probes[num_reg_probes++]=usp;
		test_printk("Number of registerd probes = %d \n",num_reg_probes);
        }
}


static void child_exit(void) 
{
	test_printk("num_probes=%ld\n", num_probes);
}


int init_module(void)
{

#ifdef UPROBES_REG_CALLBACK
	int ret;
	struct uprobe *usp;
#endif

	printk(KERN_INFO "In kunreg init_module \n");

	/* If we cant setup dbfs do not continue */
	if ( u_dbfs_init("kunreg") <  0 ){
		printk(KERN_INFO "In kunreg u_dbfs_init failed \n");
		return -1;
	}
#ifdef UPROBES_REG_CALLBACK
	/* register uprobe for calling register_all() */
	usp=(struct uprobe *)kzalloc(sizeof(struct uprobe),GFP_USER);
	if (unlikely(usp == NULL)) {
		printk(KERN_INFO "Out of memory\n");
		u_dbfs_cleanup();
		return (-ENOMEM);
	}

	usp->pid=pid;
	usp->vaddr=find_vaddr("reg_all"); 
	usp->handler=register_all;

	test_printk(
		"Registering uprobe for reg_all on pid %d, vaddr %#lx[%s]\n",
		usp->pid, usp->vaddr,"reg_all");
	ret = register_uprobe(usp);
	if ((ret != 0) && (ret != -EINPROGRESS)) {
		printk(KERN_INFO "register_uprobe() failed, returned %d\n", 
				ret);
		u_dbfs_cleanup();
		return -1;
	} 
#else /* UPROBES_REG_CALLBACK */

	register_all(NULL,NULL);
#endif /* UPROBES_REG_CALLBACK */

#ifdef UPROBES_REG_CALLBACK
	/* register uprobe for calling unregister_all() */
	usp=(struct uprobe *)kzalloc(sizeof(struct uprobe),GFP_USER);
	if (unlikely(usp == NULL)) {
		printk(KERN_INFO "Out of memory\n");
		u_dbfs_cleanup();
		return (-ENOMEM);
	}

	usp->pid=pid;
	usp->vaddr=find_vaddr("unreg_all");
	usp->handler=unregister_all;

	test_printk(
		"Registering uprobe for unreg_all on pid %d, vaddr %#lx[%s]\n",
		usp->pid, usp->vaddr,"unreg_all");

	ret = register_uprobe(usp);
	if (ret != 0) {
		printk(KERN_INFO "register_uprobe() failed, returned %d\n",
			       	ret);
		u_dbfs_cleanup();
		return -1;
	}
#endif /* UPROBES_REG_CALLBACK */
	return(0);
}

void cleanup_module(void)
{
	printk(KERN_INFO "cleanup kunreg num_probes=%ld\n", num_probes);
	unregister_all(NULL,NULL);
	u_dbfs_cleanup();
}
MODULE_LICENSE("GPL");
