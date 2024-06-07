#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>

	
struct proc_dir_entry *proc_mymounts = NULL;

static int __init init_mymounts(void)
{
	const struct proc_ops p_ops;
	proc_mymounts = proc_create("mymounts", 0600, NULL, &p_ops);
	return 0;
}

static void __exit exit_mymounts(void)
{
	proc_remove(proc_mymounts);
}

MODULE_LICENSE("GPL");
module_init(init_mymounts);
module_exit(exit_mymounts);
