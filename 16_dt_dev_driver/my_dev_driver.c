#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>

/* Name all the compatbile devices */
static const struct of_device_id my_dev_ids[] = {
	{ .compatible = "brightlight,mydev"},
	{} /* Empty Element -> end of the list */
};
MODULE_DEVICE_TABLE(of, my_dev_ids);

/* Implement a probe and a remove function */
static int my_dev_probe(struct platform_device *pdev)
{
	pr_info("my_dev - Probe Function is called!\n");
	return 0;
}

static void my_dev_remove(struct platform_device *pdev)
{
	pr_info("my_dev - Remove Function is called!\n");
}

/* Bundle compatible devices, probe and remove in driver's struct */
static struct platform_driver my_dev_driver = {
	.probe = my_dev_probe,
	.remove = my_dev_remove,
	.driver = {
		.name = "my_dev_driver",
		.of_match_table = my_dev_ids,
	}
};

/* Register the driver at the OS */
#if 1
static int __init my_init(void)
{
	pr_info("my_dev - Init function is called\n");
	return platform_driver_register(&my_dev_driver);;
}

static void __exit my_exit(void)
{
	pr_info("my_dev - Exit function is called\n");
	platform_driver_unregister(&my_dev_driver);;
}

module_init(my_init);
module_exit(my_exit);
#else
module_platform_driver(my_dev_driver);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("DT Driver for my device");
