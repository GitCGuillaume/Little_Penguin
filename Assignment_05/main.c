// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gchopin");

static ssize_t ft_read(struct file *filep,  char __user *buf,
		size_t count, loff_t *offset)
{
	int ret = 0;

	if (count <= *offset)
		return 0;
	ret = copy_to_user(buf, "gchopin\n", 8);
	if (ret)
		return -EFAULT;
	*offset += count;
	return count;
}

static ssize_t ft_write(struct file *filep, const char __user *buf,
		size_t count, loff_t *offset)
{
	char *str = NULL;
	int ret = 0;

	if (count <= *offset)
		return 0;
	str = kzalloc(count * sizeof(char) + 1, GFP_KERNEL);
	if (!str)
		return -EFAULT;
	ret = copy_from_user(str, buf, count);
	if (ret) {
		kfree(str);
		return -EFAULT;
	}
	*offset += count;
	if (count > 0 && str[count - 1] == '\n')
		str[count - 1] = 0;
	if (!strcmp(str, "gchopin")) {
		kfree(str);
		return count;
	}
	kfree(str);
	return -EINVAL;
}

const struct file_operations fops = {
		.owner = THIS_MODULE,
		.read = ft_read,
		.write = ft_write
};

struct miscdevice g_misc;

/*
 * To register a device with a minor number
 * https://www.kernel.org/doc/html/v4.13/driver-api/misc_devices.html
 */
static int __init init_hello(void)
{
	int res;

	memset(&g_misc, 0, sizeof(struct miscdevice));
	g_misc.fops = &fops;
	g_misc.minor = MISC_DYNAMIC_MINOR;
	g_misc.name = "fortytwo";
	res = misc_register(&g_misc);
	if (res != 0) {
		pr_err("Couldn't register miscellaneous device !\n");
		return 1;
	}
	pr_info("Hello world !\n");
	return 0;
}

static void __exit exit_hello(void)
{
	misc_deregister(&g_misc);
	pr_info("Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);
