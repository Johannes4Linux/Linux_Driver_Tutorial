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

struct my_dev_data {
	struct gpio_desc *led;
	struct gpio_desc *button;
};

/* Implement a probe and a remove function */
static int my_dev_probe(struct platform_device *pdev)
{
	struct my_dev_data *priv;
	int status;
	struct device *dev = &pdev->dev;

	priv = devm_kzalloc(dev, sizeof(struct my_dev_data), GFP_KERNEL);

	if (!priv) {
		dev_err(dev, "Out of memory\n");
		return -ENOMEM;
	}

	priv->led = gpiod_get(dev, "led", GPIOD_OUT_LOW);
	status = IS_ERR(priv->led);

	if (status) {
		dev_err(dev, "Could not read in led-gpio property!\n");
		return status;
	}

	priv->button = gpiod_get(dev, "button", GPIOD_IN);
	status = IS_ERR(priv->button);

	if (status) {
		dev_err(dev, "Could not read in button-gpio property!\n");
		goto free_led;
	}

	gpiod_set_value(priv->led, 1);

	status = gpiod_get_value(priv->button);
	dev_info(dev, "Button is %spressed!\n", status ? "" : "not ");

	platform_set_drvdata(pdev, priv);

	return 0;

free_led:
	gpiod_put(priv->led);
	return status;
}

static void my_dev_remove(struct platform_device *pdev)
{
	struct my_dev_data *priv = platform_get_drvdata(pdev);

	gpiod_set_value(priv->led, 0);
	gpiod_put(priv->led);
	gpiod_put(priv->button);
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
