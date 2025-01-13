#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void)
{
	printk(KERN_INFO "log_level - This is an info\n");
	printk(KERN_WARNING "log_level - This is an information\n");
	printk(KERN_ALERT "log_level - This is an alert\n");
	printk(KERN_DEBUG "log_level - Debug: %d\n", 42);
	return 0;
}

static void __exit my_exit(void)
{
	pr_emerg("log_level - Module is unloaded!\n");
	pr_err("log_level - Error\n");
	pr_info("log_level - the last info...\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("A simple Hello World Linux Kernel Module");
