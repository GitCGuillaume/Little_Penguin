// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gchopin");

struct dentry *dentry_42;
struct dentry *dentry_id;
struct dentry *dentry_jiffies;
struct dentry *dentry_foo;
struct page *page_value;
struct mutex lock;
void *virtual_address;
DEFINE_MUTEX(lock);

/*
 * write part
 */
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

/*
 * https://docs.kernel.org/admin-guide/mm/concepts.html
 * https://www.kernel.org/doc/gorman/html/understand/understand009.html
 */
static ssize_t ft_write_foo(struct file *filep, const char __user *buf,
		size_t count, loff_t *offset)
{
	int ret;

	if (count <= *offset)
		return 0;
	if (count >= PAGE_SIZE)
		return -EINVAL;
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	if (!page_value) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	virtual_address = page_address(page_value);
	if (!virtual_address) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	clear_page(virtual_address);
	ret = copy_from_user(virtual_address, buf, count);
	if (ret) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	*offset += count;
	mutex_unlock(&lock);
	return count;
}

/*
 * read part
 */
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

static size_t nb_len(unsigned long cpy)
{
	size_t len = 0;

	if (cpy == 0)
		return 1;
	while (cpy != 0) {
		++len;
		cpy /= 10;
	}
	return len;
}

static ssize_t read_jiffies(struct file *filep,  char __user *buf,
		size_t count, loff_t *offset)
{
	unsigned long cpy = jiffies;
	char *str;
	int ret;

	if (count <= *offset)
		return 0;
	str = kzalloc((nb_len(cpy) * sizeof(char)) + 2, GFP_KERNEL);
	if (!str)
		return -EFAULT;
	snprintf(str, nb_len(cpy) + 1, "%ld", cpy);
	str[nb_len(cpy)] = '\n';
	ret = copy_to_user(buf, str, nb_len(cpy) + 1);
	if (ret) {
		kfree(str);
		return -EFAULT;
	}
	*offset += count;
	kfree(str);
	return count;
}

static ssize_t ft_read_foo(struct file *filep,  char __user *buf,
		size_t count, loff_t *offset)
{
	int ret = 0;

	if (count <= *offset || !virtual_address)
		return 0;
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	if (!page_value) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	virtual_address = page_address(page_value);
	if (!virtual_address) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	ret = copy_to_user(buf, virtual_address, strlen(virtual_address));
	if (ret) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	*offset += count;
	mutex_unlock(&lock);
	return count;
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

const struct file_operations fops_foo = {
	.owner = THIS_MODULE,
	.read = ft_read_foo,
	.write = ft_write_foo
};

/*
 * https://www.kernel.org/doc/Documentation/filesystems/debugfs.rst
 */
static int init_debugfs_file(const char *name, const umode_t mode,
		struct dentry **d, const struct file_operations *f_op)
{
	if (!d)
		return 1;
	*d = debugfs_create_file(name, mode, dentry_42, NULL, f_op);
	if (!*d) {
		pr_err("Couldn't initialize debugfs device.");
		if (ERR_PTR(-ENODEV))
			return -ENODEV;
		return -EINVAL;
	}
	return 0;
}

/*
 * https://www.kernel.org/doc/Documentation/filesystems/debugfs.rst
 */
static int __init init_hello(void)
{
	int res = 0;

	page_value = NULL;
	dentry_42 = debugfs_create_dir("fortytwo", NULL);
	if (!dentry_42) {
		pr_err("Couldn't initialize fortytwo directory.");
		if (ERR_PTR(-ENODEV))
			return -ENODEV;
		return -EINVAL;
	}
	res = init_debugfs_file("id", 0666, &dentry_id, &fops);
	if (res)
		return res;
	res = init_debugfs_file("jiffies", 0444,
			&dentry_jiffies, &fops_jiffies);
	if (res)
		return res;
	res = init_debugfs_file("foo", 0644, &dentry_foo, &fops_foo);
	if (res)
		return res;
	page_value = alloc_page(GFP_KERNEL);
	if (!page_value)
		return -ENOMEM;
	pr_info("Hello world !\n");
	return 0;
}

static void __exit exit_hello(void)
{
	if (page_value)
		__free_page(page_value);
	debugfs_remove(dentry_id);
	debugfs_remove(dentry_jiffies);
	debugfs_remove(dentry_foo);
	debugfs_remove(dentry_42);
	pr_info("Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);

