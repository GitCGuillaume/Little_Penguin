#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gchopin");

struct dentry *dentry_42 = NULL;
struct dentry *dentry_id = NULL;
struct dentry *dentry_jiffies = NULL;
struct dentry *dentry_foo = NULL;
struct page *page_value = NULL;
struct mutex lock;
void *virtual_address = NULL;
DEFINE_MUTEX(lock);

/*
 * write part
*/
static ssize_t ft_write(struct file *tree, const char __user * buf,
		size_t count, loff_t *offset)
{
	char *str = NULL;
	int ret = 0;

	if (count <= *offset)
		return 0;
	str = kmalloc(count * sizeof(char) + 1, GFP_KERNEL);
	if (!str)
		return -EFAULT;
	memset(str, 0, count + 1);
	ret = copy_from_user(str, buf, count);
	//if (ret) {
	//	kfree(str);
	//	return -EFAULT;
	//}
	*offset += count - ret;
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
 op
*/
static ssize_t ft_write_foo(struct file *tree, const char __user * buf,
		size_t count, loff_t *offset)
{
	if (count <= *offset)
		return 0;
	//printk("ld:%lu\n", PAGE_SIZE);
	if (PAGE_SIZE <= count)
		return -EINVAL;
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	clear_page(virtual_address);
	int ret = copy_from_user(virtual_address, buf, count);
	//if (ret) {
	//	mutex_unlock(&lock);
	//	return -EFAULT;
	//}
	*offset += count - ret;
	mutex_unlock(&lock);
	return count;
}

/*
 * read part
*/
static ssize_t ft_read(struct file *tree,  char __user * buf,
		size_t count, loff_t *offset)
{
	int ret = 0;

	if (count <= *offset)
		return 0;
	ret = copy_to_user(buf, "gchopin\n", 8);
	//if (ret) {
	//	return -EFAULT;
	//}
	*offset += count - ret;
	return (count - ret);
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

static ssize_t read_jiffies(struct file *tree,  char __user * buf,
		size_t count, loff_t *offset)
{
	unsigned long cpy = jiffies;
	if (count <= *offset)
		return 0;
	char *str = kmalloc((nb_len(cpy) * sizeof(char)) + 2, GFP_KERNEL);
	if (!str)
		return -EFAULT;
	snprintf(str, nb_len(cpy) + 1, "%ld", cpy);
	str[nb_len(cpy)] = '\n';
	str[nb_len(cpy) + 1] = '\0';
	int ret = copy_to_user(buf, str, nb_len(cpy) + 1);
	//if (ret) {
	//	kfree(str);
	//	return -EFAULT;
	//}
	*offset += count - ret;
	kfree(str);
	return count - ret;
}

static ssize_t ft_read_foo(struct file *tree,  char __user * buf,
		size_t count, loff_t *offset)
{
	int ret = 0;

	if (count <= *offset)
		return 0;
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	ret = copy_to_user(buf, virtual_address, strlen(virtual_address));
	//if (ret) {
	//	mutex_unlock(&lock);
	//	return -EFAULT;
	//}
	*offset += count - ret;
	mutex_unlock(&lock);
	return (count - ret);
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
	*d = debugfs_create_file(name, 0666, dentry_42, NULL, *&f_op);
	if (!*d) {
		printk(KERN_ERR "Couldn't initialize debugfs device.");
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
	dentry_42 = debugfs_create_dir("fortytwo", NULL);
	if (!dentry_42) {
		printk(KERN_ERR "Couldn't initialize fortytwo directory.");
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
		return res;
	page_value = alloc_page(GFP_KERNEL);
	if (!page_value)
		return -EFAULT;
	virtual_address = page_address(page_value);
	printk(KERN_INFO "Hello world !\n");
	return 0;
}

static void __exit exit_hello(void)
{
	__free_page(page_value);
	if (dentry_id)
		debugfs_remove(dentry_id);
	if (dentry_jiffies)
		debugfs_remove(dentry_jiffies);
	if (dentry_foo)
		debugfs_remove(dentry_foo);
	if (dentry_42)
		debugfs_remove(dentry_42);
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(init_hello);
module_exit(exit_hello);

