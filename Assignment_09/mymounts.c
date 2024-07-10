#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/nsproxy.h>
#include <linux/string.h>
#include <../fs/mount.h>

//static ssize_t proc_read_mount_list(struct file *file, char __user *user,
//		size_t count, loff_t *offset);
void	recursive_name(struct seq_file *file, struct mount *mnt);
int show (struct seq_file *m, void *v);
int	ft_proc_open(struct inode *inode, struct file *file);
struct proc_dir_entry *proc_mymounts = NULL;
static DECLARE_RWSEM(namespace_sem);
//struct mutex lock;
//DEFINE_MUTEX(lock);
/*
static size_t iterate_mountpoint(struct mount *mnt, char __user **user, size_t count, loff_t *offset)
{
	struct mount *node = 0;
	char *str = 0;
	if (!mnt)
		return 0;
	printk("s:%s %s\n", mnt->mnt_devname, mnt->mnt_mountpoint->d_name.name);
	size_t len = strlen(mnt->mnt_devname) + strlen(mnt->mnt_mountpoint->d_name.name) + strlen("\t") + strlen("\n");
	str = kmalloc(sizeof(char) * (len + 1), GFP_KERNEL);
	strlcat(str, mnt->mnt_devname, sizeof(str));
	strlcat(str, "\t", sizeof(str));
	strlcat(str + (strlen(str)), mnt->mnt_mountpoint->d_name.name, sizeof(str));
	str[len - 1] = '\n';
	str[len] = '\0';
	//size_t res = simple_read_from_buffer(user, count, offset, str, strlen(str));
	size_t res = 0;
	while (len > 0 && str[res]) {
		printk("c:%c ", str[res]);
*/		//put_user(str[res], *(user)++);
		//*offset += 1;
/*		++res;
		--len;
	}
	kfree(str);
	str = NULL;
	//printk("res2:%ld\n", res2);
	return res;
*/	/*if (0 < res2)
		*res += res2;
	else if (res2 == 0){
		return ;
	} else {
		*res = res2;
	}*/
	//list_for_each_entry(node, &mnt->mnt_mounts, mnt_child) {
	//iterate_mountpoint(res, node, user, count, offset);
	//}
//}

/*
 * for current > read Linux Device Driver 3rd,chapter 2 #Quick Reference
 * https://www.kernel.org/doc/html/latest/filesystems/path-lookup.html#dcache-mounted
*/
/*static ssize_t proc_read_mount_list(struct file *file, char __user *user,
		size_t count, loff_t *offset)
{
	if (mutex_lock_interruptible(&lock))
		return -EINTR;
	struct mount *mnt = current->nsproxy->mnt_ns->root;
	struct dentry *mp = 0;
	struct mount *node = 0;
	struct mount *t = 0;
	if (count <= *offset)
	{
		mutex_unlock(&lock);
		return 0;
	}
	printk("nb: %u\n", current->nsproxy->mnt_ns->nr_mounts);
	size_t res = 0;
	size_t res2 = 0;
	list_for_each_entry(node, &mnt->mnt_mounts, mnt_child) {
		if (res >= 0)
			res = iterate_mountpoint(node, &user, count, offset);
		if (res < 0)
			res2 = -EFAULT;
		else if (res > 0)
			res2 = res;
		else
			res2 = 0;
	}
	printk("res2:%ld\n", res2);
	printk("ENDD\n");
*/	//printk("devname:%s path:%s\n", mnt->root->mnt_devname, mnt->root->mnt_mountpoint->d_name.name);
//	hlist_for_each_entry(mp, &mnt->mnt_mountpoint->d_children, d_sib)
//		printk("name:%s\n", mp->d_name.name);
	/*list_for_each_entry(node, &mnt->mnt_mounts, mnt_child) {
		printk("name:%s path:%s\n", node->mnt_devname, node->mnt_mountpoint->d_name.name);
		t = node;
	}
	list_for_each_entry(node, &t->mnt_mounts, mnt_child) {
		printk("name:%s path:%s\n", node->mnt_devname, node->mnt_mountpoint->d_name.name);
		t = node;
	}
	list_for_each_entry(node, &t->mnt_mounts, mnt_child)
		printk("name:%s path:%s\n", node->mnt_devname, node->mnt_mountpoint->d_name.name);
	*/
/*	mutex_unlock(&lock);
	return res2;
}*/
/*static ssize_t proc_read_mount_list(struct file *file, char __user *user,
		size_t count, loff_t *offset)
{
	if (count <= *offset)
		return 0;
	struct dentry *node = NULL;
	struct dentry *first = current->fs->root.mnt->mnt_root;
	struct super_block *fs_list = first->d_sb;
	struct super_block *pos = 0;
	//struct inode *d_node = first->d_inode;
        //struct inode *pos_inode = 0;
	rcu_read_lock();
	printk("c: %s %s\n", first->d_name.name, first->d_iname);
	printk("sfs: %s\n", first->d_sb->s_sysfs_name);
	printk("s_id: %s\n", first->d_sb->s_id);
	printk("type: %s\n", first->d_sb->s_type->name);
	printk("sb: %s\n", first->d_sb->s_subtype);
	printk("addr: %p\n", first->d_sb);
	//char arr[50]={0};
//	printk("s:%s\n", dentry_path_raw(current->fs->root.mnt->mnt_root, arr, 50));
	hlist_for_each_entry(node, &current->fs->root.mnt->mnt_root->d_children, d_sib) {
//		if (d_mountpoint(node)) {
			printk("c: %s %s\n", node->d_name.name, node->d_iname);
			printk("sfs: %s\n", node->d_sb->s_sysfs_name);
			printk("fl:%d\n", node->d_flags);
			printk("s_id: %s\n", node->d_sb->s_id);
			printk("type_name: %s\n", &*node->d_sb->s_type->name);
			printk("subt: %s\n", node->d_sb->s_subtype);
			printk("addr: %p\n", &node->d_sb);
			printk("addr: %p\n", node->d_sb);
//		}
	}
	printk("s_id: %s\n", fs_list->s_id);
	printk("s_name: %s\n", fs_list->s_type->name);
	printk("s_sut: %s\n", fs_list->s_subtype);
	list_for_each_entry(pos, &fs_list->s_list, s_list) {
		printk("s_id: %s\n", pos->s_id);
		printk("s_name: %s\n", pos->s_type->name);
		printk("s_sut: %s\n", pos->s_subtype);
	}

	rcu_read_unlock();
	
	return 0;
}*/

/*
 * https://docs.kernel.org/filesystems/seq_file.html
*/
void	recursive_name(struct seq_file *file, struct mount *mnt)
{
	struct path pat;
	pat.mnt = &mnt->mnt;
	pat.dentry = mnt->mnt_mountpoint;

	seq_puts(file, mnt->mnt_devname);
	seq_putc(file, ' ');
	seq_path(file, &pat, "");
	seq_puts(file, mnt->mnt_mountpoint->d_name.name);
	seq_putc(file, '\n');
	struct dentry *det = mnt->mnt_mountpoint;
	char buf[500];
	memset(buf, 0, sizeof(buf));
	if (mnt->mnt_mountpoint->d_sb->s_op->show_devname)
		mnt->mnt_mountpoint->d_sb->s_op->show_devname(file, det);
	seq_path(file, &pat, "");
	seq_puts(file, dentry_path_raw(det, buf, sizeof(buf)));
	seq_putc(file, '\n');
	if (mnt->mnt_mp && mnt->mnt_mp->m_dentry) {
		memset(buf, 0, sizeof(buf));
		seq_path(file, &pat, "");
		seq_puts(file, dentry_path_raw(mnt->mnt_mountpoint->d_parent->d_parent, buf, sizeof(buf)));
		memset(buf, 0, sizeof(buf));
		seq_puts(file, dentry_path_raw(mnt->mnt_mp->m_dentry, buf, sizeof(buf)));
		seq_putc(file, '\n');
	}
	memset(buf, 0, sizeof(buf));
	printk("par:%d\n",  mnt_has_parent(mnt));
	//printk("name:%s\n", mnt->mnt_devname);
	/*size_t len = strlen(mnt->mnt_devname) + strlen(mnt->mnt_mountpoint->d_name.name)
		     + strlen(" ") + strlen("\n");
	char *str = kmalloc(sizeof(char) * (len + 1), GFP_KERNEL);
	str[0] = 0;
	//printk("strlen_sizeof:%ld\n", sizeof(str));
	strlcat(str, mnt->mnt_devname, len + 1);
	//printk("strlen:%ld\n", strlen(str));
	//printk("strlen:%ld\n", strlen(mnt->mnt_devname));
	//printk("strlen:%ld\n", len);
	strlcat(str, " ", len + 1);
	strlcat(str, mnt->mnt_mountpoint->d_name.name, len + 1);
	strlcat(str, "\n", len + 1);
	//str[len - 1] = '\n';
	//str[len] = '\0';
	struct path pat;
	pat.mnt = &mnt->mnt;
	pat.dentry = mnt->mnt_mountpoint;
	seq_path(file, &pat, "");
	seq_puts(file, str);
	mnt->mnt_mountpoint->d_op->d_dname = d_absolute_path;
//int (*show_path)(struct seq_file *, struct dentry *);
	kfree(str);
	*/
}

int show (struct seq_file *file, void *private)
{
	//if (mutex_lock_interruptible(&lock))
	//	return -EINTR;
	struct mnt_namespace *mnt_ns = current->nsproxy->mnt_ns;
	struct mount *node = 0;
	struct mount *tmp = 0;
	
	down_read(&namespace_sem);	
	rbtree_postorder_for_each_entry_safe(node, tmp, &mnt_ns->mounts, mnt_node) {
			recursive_name(file, node);
	}
	up_read(&namespace_sem);
	//rb_for_each(node, key, tree, cmp)
	//rbtree_postorder_for_each_entry_safe(node, next, &mnt_ns->rb_root, rb_node) {
	//	printk("val:%s\n", node->mnt_devname);
	//}
	
	//struct mount *node2 = 0;
	//list_for_each_entry(node, &mnt->mnt_child, mnt_child) {
	//	printk("val:%s\n", node->mnt_devname);
		//if (node) {
		//	node2 = node;
		//}
		//recursive_name(file, node);
	//}
	/*list_for_each_entry(node, &node2->mnt_mounts, mnt_child) {
		if (node) {
			node2 = node;
		}
		recursive_name(file, node);
	}*/
	//mutex_unlock(&lock);
	return 0;
}

int	ft_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, show, NULL);
}

//.proc_read = proc_read_mount_list
const struct proc_ops p_ops = {
	.proc_open = ft_proc_open,
	.proc_read = seq_read,
	.proc_release = single_release,
};

static int __init init_mymounts(void)
{
	proc_mymounts = proc_create("mymounts", 0600, NULL, &p_ops);
	if (!proc_mymounts)
		return 1;
	return 0;
}

static void __exit exit_mymounts(void)
{
	proc_remove(proc_mymounts);
}

MODULE_LICENSE("GPL");
module_init(init_mymounts);
module_exit(exit_mymounts);
