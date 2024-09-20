// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/nsproxy.h>
#include <../fs/mount.h>

struct	proc_dir_entry *proc_mymounts;
static	DECLARE_RWSEM(namespace_sem);

/*
 * https://docs.kernel.org/filesystems/seq_file.html
 */
static void	recursive_name(struct seq_file *file, struct mount *mnt)
{
	struct path show_path;

	show_path.mnt = &mnt->mnt;
	show_path.dentry = mnt->mnt_mountpoint;
	seq_puts(file, mnt->mnt_devname);
	seq_putc(file, ' ');
	if (mnt->mnt_parent) {
		show_path.mnt = &mnt->mnt_parent->mnt;
		show_path.dentry = mnt->mnt_mountpoint;
		seq_path(file, &show_path, "");
	}
	seq_putc(file, '\n');
}

static int show(struct seq_file *file, void *private)
{
	down_read(&namespace_sem);
	struct mnt_namespace *mnt_ns = current->nsproxy->mnt_ns;
	struct rb_root *root = &mnt_ns->mounts;
	struct mount *mnt = NULL;

	for (mnt = rb_entry_safe(rb_first(root), struct mount, mnt_node);
		mnt != NULL;
		mnt = rb_entry_safe(rb_next(&mnt->mnt_node),
			struct mount, mnt_node)) {
		if (strcmp(mnt->mnt_devname, "rootfs") != 0)
			recursive_name(file, mnt);
	}
	up_read(&namespace_sem);
	return 0;
}

static int	ft_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, show, NULL);
}

const struct proc_ops p_ops = {
	.proc_open = ft_proc_open,
	.proc_read = seq_read,
	.proc_release = single_release,
};

static int __init init_mymounts(void)
{
	proc_mymounts = proc_create("mymounts", 0444, NULL, &p_ops);
	if (!proc_mymounts)
		return -ENOMEM;
	return 0;
}

static void __exit exit_mymounts(void)
{
	proc_remove(proc_mymounts);
}

MODULE_LICENSE("GPL");
module_init(init_mymounts);
module_exit(exit_mymounts);
