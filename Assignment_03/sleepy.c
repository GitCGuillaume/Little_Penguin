#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>

MODULE_LICENSE("42");

/*
 * Tell userspace about long sleep
*/
static void sleep_info(void)
{
	pr_info("We slept a long time!");
}

static int do_work(int *my_int)
{
	int	i;
	int	j = *my_int;
	int	result;

	for (i = 0; i < *my_int; ++i)
		udelay(10);
	if (j < 10) {
		sleep_info();
		result = i * j;
	}
	return 0;
}

static int my_init(void)
{
	int x = 10;

	x = do_work(&x);
	return x;
}

static void	my_exit(void)
{
	return ;
}

module_init(my_init);
module_exit(my_exit);
