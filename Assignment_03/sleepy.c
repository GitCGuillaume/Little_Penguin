// SPDX-License-Identifier: GPL-3.0-or-later
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

/*
 * Try to sleep, tell userspace about long sleep
 */
static int do_work(int *my_int)
{
	int	i;
	int	j = *my_int;
	int	k;

	for (i = 0; i < *my_int; ++i)
		udelay(10);
	if (j < 10)
		pr_info("We slept a long time!");
	k = i * j;
	return k;
}

static int my_init(void)
{
	int x = 10;

	x = do_work(&x);
	return x;
}

static void	my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);
