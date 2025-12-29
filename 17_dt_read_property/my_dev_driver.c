#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/property.h>

/* Name all the compatbile devices */
static const struct of_device_id my_dev_ids[] = {
	{ .compatible = "brightlight,mydev"},
	{} /* Empty Element -> end of the list */
};
MODULE_DEVICE_TABLE(of, my_dev_ids);

/* Implement a probe and a remove function */
static int my_dev_probe(struct platform_device *pdev)
{
	int status;
	u32 int_var;
	const char *string_var;

	struct device *dev = &pdev->dev;


	if (!device_property_present(dev, "int_var")) {
		pr_err("my_dev - int_var property is not present!\n");
		return -1;
	}

	if (!device_property_present(dev, "string_var")) {
		pr_err("my_dev - string_var property is not present!\n");
		return -1;
	}

	status = device_property_read_u32(dev, "int_var", &int_var);

	if (status) {
		pr_err("my_dev - error reading int_var property\n");
		return status;
	}

	pr_info("my_dev - int_var: %d\n", int_var);

	status = device_property_read_string(dev, "string_var", &string_var);

	if (status) {
		pr_err("my_dev - error reading string_var property\n");
		return status;
	}

	pr_info("my_dev - string_var: %s\n", string_var);
		
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
