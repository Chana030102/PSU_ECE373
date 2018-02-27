#include <linux/module.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/delay.h>

static int __init sleep_example_init(void)
{
	printk(KERN_INFO "before msleep: %lu\n", jiffies);
	msleep(1000);
	printk(KERN_INFO "after msleep: %lu\n", jiffies);

	return 0;
}

static void __exit sleep_example_exit(void)
{
	printk(KERN_INFO "unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PJ Waskiewicz");
MODULE_VERSION("0.1");

module_init(sleep_example_init);
module_exit(sleep_example_exit);
