#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>

/* Name all the compatbile devices */
static const struct of_device_id my_dev_ids[] = {
	{ .compatible = "brightlight,mydev"},
	{} /* Empty Element -> end of the list */
};
MODULE_DEVICE_TABLE(of, my_dev_ids);

static struct gpio_desc *led, *button;

/* Implement a probe and a remove function */
static int my_dev_probe(struct platform_device *pdev)
{
	int status;
	struct device *dev = &pdev->dev;

	led = gpiod_get(dev, "led", GPIOD_OUT_LOW);
	status = IS_ERR(led);

	if (status) {
		dev_err(dev, "Could not read in led-gpio property!\n");
		return status;
	}

	button = gpiod_get(dev, "button", GPIOD_IN);
	status = IS_ERR(button);

	if (status) {
		dev_err(dev, "Could not read in button-gpio property!\n");
		goto free_led;
	}

	gpiod_set_value(led, 1);

	status = gpiod_get_value(button);
	dev_info(dev, "Button is %spressed!\n", status ? "" : "not ");

	return 0;

free_led:
	gpiod_put(led);
	return status;
}

static void my_dev_remove(struct platform_device *pdev)
{
	gpiod_set_value(led, 0);
	gpiod_put(led);
	gpiod_put(button);
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
module_platform_driver(my_dev_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("DT Driver for my device");
