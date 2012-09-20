#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uprobes.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

//#define DEBUG 1

/* FIXME move into a header */
struct uinfo {
	pid_t utid,ktid;
	ulong ucount, kcount;
	int results;
};

#ifdef __x86_64__

#define REG_IP(regs) regs->rip
#define REG_SP(regs) regs->rsp

ulong *get_first_arg(struct pt_regs * regs)
{
	// place holder
	return NULL;
}

#elif defined (__i386__)

#define REG_IP(regs) regs->eip
#define REG_SP(regs) regs->esp
#define REG_0(regs)  regs->eax
#define REG_1(regs)  regs->ecx
#define REG_2(regs)  regs->edx
#define REG_3(regs)  regs->ebx
#define REG_5(regs)  regs->ebp
#define REG_6(regs)  regs->esi
#define REG_7(regs)  regs->edi

void printk_regs(struct pt_regs * regs)
{
	test_printk ("EIP: %08lx\n",REG_IP(regs));
	test_printk ("ESP: %08lx\n",REG_SP(regs));
	test_printk ("EAX: %08lx EBX: %08lx ECX: %08lx EDX: %08lx\n",
		REG_0(regs),REG_3(regs),REG_1(regs),REG_2(regs));
	test_printk ("ESI: %08lx EDI: %08lx EBP: %08lx\n",
		REG_6(regs), REG_7(regs), REG_5(regs));
}

ulong *get_first_arg(struct pt_regs * regs)
{
	ulong *sp=(ulong *)REG_SP(regs);
	sp++;
	return (ulong *)*sp;
}

#elif defined (__ia64__)
// FIXME
#define REG_IP(regs)    ((regs)->cr_iip +ia64_psr(regs)->ri)
#define REG_SP(regs)    ((regs)->r12)

ulong *get_first_arg(struct pt_regs * regs)
{
	// place holder
	return NULL;
}

#elif defined (__powerpc64__)

// FIXME
#define REG_IP(regs) regs->nip
#define REG_SP(regs) regs->gpr[1]
#define REG_LINK(regs) regs->link

ulong *get_first_arg(struct pt_regs * regs)
{
	// place holder
	return NULL;
}


#elif defined (__s390__) || defined (__s390x__)
//FIXME
#ifndef __s390x__
#define PSW_ADDR_AMODE  0x80000000UL
#else /* __s390x__ */
#define PSW_ADDR_AMODE  0x0000000000000000UL
#endif /* __s390x__ */
#define REG_IP(regs)    ((regs)->psw.addr) | PSW_ADDR_AMODE
#define REG_SP(regs)    (regs)->gprs[15]

ulong *get_first_arg(struct pt_regs * regs)
{
	// place holder
	return NULL;
}

#else
#error "Unimplemented architecture"
#endif


/* The main job of the probe handler is to increment the uinfo->kcount seen by the
 * user.  It is up to the user thread to decide if the value of kcount is correct or
 * not.  Other values may be sanity checked or set here also.
 */

unsigned long num_probes;

int probe_handler(struct uprobe_consumer *u, struct pt_regs *regs)
{
		struct uinfo *slot;
		pid_t utid,ktid;
		ulong kcount,EA;
		
#ifdef DEBUG
		test_printk("---------------------------------------------------\n");
		test_printk("probe_handler() called for pid=%d\n",current->pid);
		test_printk_regs(regs);
		test_printk("---------------------------------------------------\n");
#endif

		slot = (struct uinfo *)get_first_arg(regs);
#ifdef DEBUG
		test_printk("slot=%p\n",slot);
#endif

		/* save current->pid in slot->ktid */
		ktid=current->pid;
		EA=offsetof(struct uinfo,ktid)+(ulong)slot;
		if ( put_user(ktid,(ulong __user *)EA) != 0 ){
			test_printk(" put_user returned an error!\n");
		}

		/* sanity check that ktid==utid */
		EA=offsetof(struct uinfo,utid)+(ulong)slot;
		if ( get_user(utid,(ulong __user *)EA) != 0 ){
			test_printk(" get_user returned an error!\n");
		}

		if (ktid != utid){
			test_printk("kthread: ERROR ktid and utid don't match!! ");
			test_printk("kthread: utid=%d ktid=%d\n",utid,ktid);
		}

		/* get the slot->kcount */
		EA=offsetof(struct uinfo,kcount)+(ulong)slot;
		if ( get_user(kcount,(ulong __user *)EA) != 0 )
			test_printk(" get_user returned an error!\n");

		/* If slot->kcount == -1 then this is the first time
		 * this probe point has been hit so set slot->ucount=-1
		 * to flag the user thread that probe counting has started.
		 */
		if (kcount == -1){
			EA=offsetof(struct uinfo,ucount)+(ulong)slot;
			if ( put_user(kcount,(ulong __user *)EA) != 0 )
				test_printk(" put_user returned an error!\n");
		}

		/* incement the slot->kcount */
		kcount++;
		EA=offsetof(struct uinfo,kcount)+(ulong)slot;
		if ( put_user(kcount,(ulong __user *)EA) != 0 )
			test_printk(" put_user returned an error!\n");

		num_probes++;
		return 0;
}

