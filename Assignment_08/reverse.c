// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

static ssize_t my_fd_read(struct file *fp, char __user *user,
		size_t size, loff_t *offs);
static ssize_t my_fd_write(struct file *fp, const char __user *user,
		size_t size, loff_t *offs);
static const struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &my_fd_read,
	.write = &my_fd_write
};
static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};
struct mutex lock;
char str[PAGE_SIZE];

DEFINE_MUTEX(lock);

static ssize_t my_fd_read(struct file *fp, char __user *user,
		size_t size, loff_t *offs)
{
	ssize_t t, i;
	ssize_t res;
	char *tmp;

	if (size <= *offs)
		return 0;
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	tmp = kmalloc(sizeof(char) * PAGE_SIZE, GFP_KERNEL);
	if (!tmp)
		return -EFAULT;
	for (t = strlen(str) - 1, i = 0; t >= 0; t--, i++)
		tmp[i] = str[t];
	tmp[i] = 0x0;
	res = simple_read_from_buffer(user, size, offs, tmp, i);
	kfree(tmp);
	tmp = 0;
	mutex_unlock(&lock);
	if (res < 0)
		return -EFAULT;
	return res;
}

static ssize_t my_fd_write(struct file *fp, const char __user *user,
		size_t size, loff_t *offs)
{
	ssize_t res;

	if (size >= PAGE_SIZE)
		return -EINVAL;
	if (size <= *offs)
		return 0;
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	memset(str, 0, PAGE_SIZE);
	res = simple_write_to_buffer(str, PAGE_SIZE, offs, user, size);
	if (res < 0) {
		memset(str, 0, PAGE_SIZE);
		return -EFAULT;
	}
	str[res] = '\0';
	mutex_unlock(&lock);
	return res;
}

static int __init my_fd_init(void)
{
	int retval = misc_register(&myfd_device);

	if (retval)
		return 1;
	memset(str, 0, PAGE_SIZE);
	return 0;
}

static void __exit my_fd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");
module_init(my_fd_init);
module_exit(my_fd_cleanup);
