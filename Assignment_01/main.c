// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gchopin");

static int __init init_hello(void)
{
	pr_info("Hello world !\n");
	return 0;
}

static void __exit exit_hello(void)
{
	pr_info("Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);

