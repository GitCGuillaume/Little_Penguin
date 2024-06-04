#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

// Dont have a license, LOL
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs);
static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};
static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};
char str[PAGE_SIZE];
char *tmp;

/*
* Malloc like a boss
*/
ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs) {
	ssize_t t, i;
	ssize_t res;
	char *tmp2;

	if (size <= *offs)
		return 0;
	tmp2 = kmalloc(sizeof(char) * PAGE_SIZE, GFP_KERNEL);
	if (!tmp2)
		return -EFAULT;
	tmp = tmp2;
	printk("size: %lu", size);
	for (t = strlen(str) - 1, i = 0; t >= 0; t--, i++) {
		tmp[i] = str[t];
	}
	printk("t: %ld\ni:%ld", t, i);
	tmp[i] = 0x0;
	printk("s:%s\n", tmp);
	printk("offs:%lld\n", *offs);
	res = simple_read_from_buffer(user, size, offs, tmp, i);
	printk("offs:%lld\nres:%ld\n", *offs, res);
	kfree(tmp2);
	tmp2 = 0;
	tmp = 0;
	if (res < 0)
		return -EFAULT;
	return res;
}

/*
 * 0x0 = ’\0’
*/
ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs) {
	ssize_t res;

	if (PAGE_SIZE <= size)
		return -EINVAL;
	if (size <= *offs)
		return 0;
	memset(str, 0, PAGE_SIZE);
	res = simple_write_to_buffer(str, size, offs, user, size);// + 1;
	if (res < 0) {
		memset(str, 0, PAGE_SIZE);
		return -EFAULT;
	}
	str[res] = 0x0;
	printk("res write:%ld\n", res);
	return res;
}

static int __init myfd_init(void) {
	int retval = misc_register(&myfd_device);
	if (retval)
		return 1;
	printk(KERN_INFO "start\n");
	tmp = (void *)0;
	memset(str, 0, PAGE_SIZE);
	return 0;
}

static void __exit myfd_cleanup(void) {
	misc_deregister(&myfd_device);
	printk(KERN_INFO "end\n");
}

module_init(myfd_init);
module_exit(myfd_cleanup);
