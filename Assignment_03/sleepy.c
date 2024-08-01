#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>

MODULE_LICENSE("42");

/*
 * That was a long sleep, tell userspace about it
*/
static int sleep_info(int x, int y)
{
	pr_info("We slept a long time!");
	return x * y;
}

static int do_work(int *my_int, int retval)
{
	int	x;
	int	y = *my_int;
	int	z;

	for (x = 0; x < *my_int; ++x) {
		udelay(10);
	}
	if (y < 10) {
		z = sleep_info(x, y);
		return z;
	}
	return 0;
}

static int my_init(void)
{
	int x = 10;

	x = do_work(&x, x);
	return x;
}

static void	my_exit(void)
{
	return ;
}

module_init(my_init);
module_exit(my_exit);
