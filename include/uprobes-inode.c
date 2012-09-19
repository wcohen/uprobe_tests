/*
 * a horrible kludge borrowed from systemtap to work around 
 * the unexported uprobe_register and uprobe_unregister
 */

#ifndef _UPROBES_INODE_C_
#define _UPROBES_INODE_C_

#include <linux/kallsyms.h>

// PR13489, inode-uprobes sometimes lacks the necessary SYMBOL_EXPORT's.
#if !defined(STAPCONF_TASK_USER_REGSET_VIEW_EXPORTED)
void *kallsyms_task_user_regset_view;
#endif
#if !defined(STAPCONF_UPROBE_REGISTER_EXPORTED)
void *kallsyms_uprobe_register;
#endif
#if !defined(STAPCONF_UPROBE_UNREGISTER_EXPORTED)
void *kallsyms_uprobe_unregister;
#endif
#if !defined(STAPCONF_UPROBE_GET_SWBP_ADDR_EXPORTED)
void *kallsyms_uprobe_get_swbp_addr;
#endif


// PR13489, inodes-uprobes export kludge
#if !defined(CONFIG_UPROBES)
#error "not to be built without CONFIG_UPROBES"
#endif
#if !defined(STAPCONF_UPROBE_REGISTER_EXPORTED)
// First get the right typeof(name) that's found in uprobes.h
#if defined(STAPCONF_OLD_INODE_UPROBES)
typedef typeof(&register_uprobe) uprobe_register_fn;
#else
typedef typeof(&uprobe_register) uprobe_register_fn;
#endif
// Then define the typecasted call via function pointer
#define uprobe_register (* (uprobe_register_fn)kallsyms_uprobe_register)
#elif defined(STAPCONF_OLD_INODE_UPROBES)
// In this case, just need to map the new name to the old
#define uprobe_register register_uprobe
#endif

#if !defined(STAPCONF_UPROBE_UNREGISTER_EXPORTED)
// First get the right typeof(name) that's found in uprobes.h
#if defined(STAPCONF_OLD_INODE_UPROBES)
typedef typeof(&unregister_uprobe) uprobe_unregister_fn;
#else
typedef typeof(&uprobe_unregister) uprobe_unregister_fn;
#endif
// Then define the typecasted call via function pointer
#define uprobe_unregister (* (uprobe_unregister_fn)kallsyms_uprobe_unregister)
#elif defined(STAPCONF_OLD_INODE_UPROBES)
// In this case, just need to map the new name to the old
#define uprobe_unregister unregister_uprobe
#endif

#if !defined(STAPCONF_UPROBE_GET_SWBP_ADDR_EXPORTED)
// First typedef from the original decl, then #define it as a typecasted call.
typedef typeof(&uprobe_get_swbp_addr) uprobe_get_swbp_addr_fn;
#define uprobe_get_swbp_addr (* (uprobe_get_swbp_addr_fn)kallsyms_uprobe_get_swbp_addr)
#endif


static int _init_uprobe(void)
{
/* PR13489, missing inode-uprobes symbol-export workaround */
#if !defined(STAPCONF_TASK_USER_REGSET_VIEW_EXPORTED) && !defined(STAPCONF_UTRACE_REGSET) /* RHEL5 era utrace */
        kallsyms_task_user_regset_view = (void*) kallsyms_lookup_name ("task_user_regset_view");
        /* There exist interesting kernel versions without task_user_regset_view(), like ARM before 3.0.
           For these kernels, uprobes etc. are out of the question, but plain kernel stap works fine.
           All we have to accomplish is have the loc2c runtime code compile.  For that, it's enough
           to leave this pointer zero. */
        if (kallsyms_task_user_regset_view == NULL) {
                ;
        }
#endif
#if defined(CONFIG_UPROBES) // i.e., kernel-embedded uprobes
#if !defined(STAPCONF_UPROBE_REGISTER_EXPORTED)
        kallsyms_uprobe_register = (void*) kallsyms_lookup_name ("uprobe_register");
        if (kallsyms_uprobe_register == NULL) {
		kallsyms_uprobe_register = (void*) kallsyms_lookup_name ("register_uprobe");
        }
        if (kallsyms_uprobe_register == NULL) {
                printk(KERN_ERR "%s can't resolve uprobe_register!", THIS_MODULE->name);
                goto err0;
        }
#endif
#if !defined(STAPCONF_UPROBE_UNREGISTER_EXPORTED)
        kallsyms_uprobe_unregister = (void*) kallsyms_lookup_name ("uprobe_unregister");
        if (kallsyms_uprobe_unregister == NULL) {
		kallsyms_uprobe_unregister = (void*) kallsyms_lookup_name ("unregister_uprobe");
        }
        if (kallsyms_uprobe_unregister == NULL) {
                printk(KERN_ERR "%s can't resolve uprobe_unregister!", THIS_MODULE->name);
                goto err0;
        }
#endif
#if !defined(STAPCONF_UPROBE_GET_SWBP_ADDR_EXPORTED)
        kallsyms_uprobe_get_swbp_addr = (void*) kallsyms_lookup_name ("uprobe_get_swbp_addr");
        if (kallsyms_uprobe_get_swbp_addr == NULL) {
                printk(KERN_ERR "%s can't resolve uprobe_get_swbp_addr!", THIS_MODULE->name);
                goto err0;
        }
#endif
#endif
	return 0;

err0:
	return -1;

}

#endif
