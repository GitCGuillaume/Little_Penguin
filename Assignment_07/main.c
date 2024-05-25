#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gchopin");

struct dentry *dentry_42 = (void *)0;
struct dentry *dentry_id = (void *)0;
struct dentry *dentry_jiffies = (void *)0;
//unsigned long volatile jiffies;

static ssize_t ft_write(struct file *tree, const char __user * buf,
		size_t count, loff_t *offset) {
	char *str = (void *)0;
	size_t len = count;
	int ret = 0;

	str = kmalloc(count * sizeof(char) + 1, GFP_KERNEL);
	if (!str)
		return EFAULT;
	memset(str, 0, count + 1);
	if (count < *offset) {
		kfree(str);
		return 0;
	}
	ret = copy_from_user(str, buf, count);
	if (ret) {
		kfree(str);
		return EFAULT;
	}
	*offset += count - ret;
	if (count > 0 && str[count - 1] == '\n')
		--len;
	if (len < 7)
		len = 7;
	if (!memcmp(str, "gchopin", len)) {
		kfree(str);
		return count;
	}
	kfree(str);
	return -EINVAL;
}

static ssize_t ft_read(struct file *tree,  char __user * buf,
		size_t count, loff_t *offset) {
	int ret = 0;

	if (count <= *offset)
		return 0;
	ret = copy_to_user(buf, "gchopin\n", 8);
	if (ret) {
		return -EFAULT;
	}
	*offset += count - ret;
	return (count - ret);
}

static ssize_t read_jiffies(struct file *tree,  char __user * buf,
		size_t count, loff_t *offset) {
	unsigned long cpy = 0;
	int ret = 0;
	if (count <= *offset)
		return 0;
	printk("count: %ld\n", count);
	static size_t i = 0;
	if (i == 1)
		return 0;
	if (cpy == 0) {
		ret = put_user(0 + '0', buf++);
		if (ret)
			return -EFAULT;
		(*offset)++;
		i++;
		count--;
		return i;
	}
	return 0;
}

const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = ft_read,
	.write = ft_write
};

const struct file_operations fops_jiffies = {
	.owner = THIS_MODULE,
	.read = read_jiffies,
};


static int __init init_hello(void)
{
	dentry_42 = debugfs_create_dir("fortytwo", NULL);
	if (!dentry_42) {
		printk(KERN_ERR "Couldn't initialize fortytwo directory.");
		return 1;
	}
	dentry_id = debugfs_create_file("id", 0666, dentry_42, NULL, &fops);
	if (!dentry_id) {
		printk(KERN_ERR "Couldn't initialize id debugfs device.");
		return 1;
	}
	dentry_jiffies = debugfs_create_file("jiffies", 0222, dentry_42, NULL, &fops_jiffies);
	if (!dentry_jiffies) {
		printk(KERN_ERR "Couldn't initialize jiffies debugfs device.");
		return 1;
	}
	printk(KERN_INFO "Hello world !\n");
	return 0;
}

static void __exit exit_hello(void)
{
	debugfs_remove(dentry_id);
	debugfs_remove(dentry_jiffies);
	debugfs_remove(dentry_42);
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);

