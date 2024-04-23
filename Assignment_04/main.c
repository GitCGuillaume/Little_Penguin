#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("42");
MODULE_AUTHOR("gchopin");

static int __init init_hello(void)
{
	printk(KERN_INFO "Hello world !\n");
	return (0);
}

static void __exit exit_hello(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);

