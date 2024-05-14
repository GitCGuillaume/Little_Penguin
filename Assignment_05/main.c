#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("42");
MODULE_AUTHOR("gchopin");

struct miscdevice g_misc;

static ssize_t ft_read(struct file *rd,  char __user * u, size_t count, loff_t *loff) {
	return (0);
}

static ssize_t ft_write(struct file *wr, const char __user * u, size_t count, loff_t *loff) {
	return (0);
}

/*
 * To register a device with a minor number
 * https://www.kernel.org/doc/html/v4.13/driver-api/misc_devices.html
*/
static int __init init_hello(void)
{
	struct file_operations fops = {
		.owner = THIS_MODULE,
		.read = ft_read,
		.write = ft_write
	};
	memset(&g_misc, 0, sizeof(struct miscdevice));
	g_misc.fops = &fops;
	g_misc.minor = MISC_DYNAMIC_MINOR;
	g_misc.name = "c_hello_world";
	int res = misc_register(&g_misc);
	if (res != 0) {
		printk(KERN_ERR "Couldn't register miscellaneous device !\n");
		return (1);
	}
	printk(KERN_INFO "Hello world !\n");
	return (0);
}

static void __exit exit_hello(void)
{
	misc_deregister(&g_misc);
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);

