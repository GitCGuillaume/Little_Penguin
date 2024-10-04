// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gchopin");

/*
 * mutex lock for foo devices
 */
struct mutex lock;
struct dentry *dentry_42;
struct dentry *dentry_id;
struct dentry *dentry_jiffies;
struct dentry *dentry_foo;
struct page *page_value;
void *virtual_address;

/*
 * init mutex
 */
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
	if (copy_from_user(virtual_address, buf, count)) {
		mutex_unlock(&lock);
		return -EFAULT;
	}
	*((char *)virtual_address + count) = 0;
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

/*
 * return integer length + \n
 */
static int	ft_intlen(u64 value)
{
	int len = 1;

	if (value > 0) {
		len = 0;
		while (value > 0) {
			value /= 10;
			++len;
		}
	}
	++len;
	return len;
}

static ssize_t read_jiffies(struct file *filep,  char __user *buf,
			    size_t count, loff_t *offset)
{
	u64	tmp = get_jiffies_64();
	char	*str;
	int	len;
	int	ret;

	if (count <= *offset)
		return 0;
	len = ft_intlen(tmp);
	str = kzalloc((sizeof(char) * len) + 1, GFP_KERNEL);
	if (snprintf(str, len, "%llu", tmp) < 0) {
		kfree(str);
		return -EFAULT;
	}
	str[len - 1] = '\n';
	ret = copy_to_user(buf, str, len);
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
	if (copy_to_user(buf, virtual_address, strlen(virtual_address))) {
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
static int init_debugfs_file_2(const char *name, const umode_t mode,
			       struct dentry **d,
			       const struct file_operations *f_op)
{
	if (!name || !d || !f_op)
		return -1;
	*d = debugfs_create_file(name, mode, dentry_42, NULL, f_op);
	if (!*d) {
		pr_err("Couldn't initialize debugfs device.");
		if (ERR_PTR(-ENODEV))
			return -ENODEV;
		return -EINVAL;
	}
	return 0;
}

static int init_debugfs_file_1(void)
{
	int res = init_debugfs_file_2("id", 0666, &dentry_id, &fops);

	if (res) {
		__free_page(page_value);
		debugfs_remove(dentry_42);
		return res;
	}
	res = init_debugfs_file_2("jiffies", 0444,
				  &dentry_jiffies, &fops_jiffies);
	if (res) {
		__free_page(page_value);
		debugfs_remove(dentry_id);
		debugfs_remove(dentry_42);
		return res;
	}
	res = init_debugfs_file_2("foo", 0644, &dentry_foo, &fops_foo);
	if (res) {
		__free_page(page_value);
		debugfs_remove(dentry_id);
		debugfs_remove(dentry_jiffies);
		debugfs_remove(dentry_42);
		return res;
	}
	return 0;
}

/*
 * https://www.kernel.org/doc/Documentation/filesystems/debugfs.rst
 */
static int __init init_hello(void)
{
	int	res;

	page_value = alloc_page(GFP_KERNEL);
	if (!page_value)
		return -ENOMEM;
	dentry_42 = debugfs_create_dir("fortytwo", NULL);
	if (!dentry_42) {
		__free_page(page_value);
		pr_err("Couldn't initialize fortytwo directory.");
		if (ERR_PTR(-ENODEV))
			return -ENODEV;
		return -EINVAL;
	}
	res = init_debugfs_file_1();
	if (!res)
		pr_info("Hello world !\n");
	return res;
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

