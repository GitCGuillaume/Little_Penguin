#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("42");
MODULE_AUTHOR("gchopin");

struct miscdevice g_misc;

static ssize_t ft_read(struct file *tree,  char __user * buf, size_t count, loff_t *offset) {
	//copy_to_user
	int ret = 0;
	ret = copy_to_user(buf, tree->private_data, count);
	//printk("test: %d %s", ret, buf);
	//if < 0
	*offset += count;
	return (ret);
}

static ssize_t ft_write(struct file *tree, const char __user * buf, size_t count, loff_t *offset) {
	int ret = 0;
	//copy from user
	ret = copy_from_user(tree->private_data, buf, count);
	//printk("test: %d %s", ret, buf);
	//if < 0
	*offset += count;
	return (ret);
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
	g_misc.name = "fortytwo";
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

