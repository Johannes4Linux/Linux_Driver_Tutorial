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

static ssize_t button_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct my_dev_data *priv = platform_get_drvdata(pdev);

	int status = gpiod_get_value(priv->button);

	return sysfs_emit(buf, "Button is %spressed!\n", status ? "" : "not ");
}

DEVICE_ATTR_RO(button);

static ssize_t led_store(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct my_dev_data *priv = platform_get_drvdata(pdev);
	int value;

	if (count < 1)
		return -EINVAL;

	value = buf[0] == '1';

	gpiod_set_value(priv->led, value);

	return count;
}

DEVICE_ATTR_WO(led);

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

	status = device_create_file(dev, &dev_attr_led);

	if (status) {
		dev_err(dev, "Could not create LED attribute\n");
		goto free_button;
	}

	status = device_create_file(dev, &dev_attr_button);

	if (status) {
		dev_err(dev, "Could not create button attribute\n");
		goto free_led_attr;
	}
	platform_set_drvdata(pdev, priv);

	return 0;
free_led_attr:
	device_remove_file(dev, &dev_attr_led);
free_button:
	gpiod_put(priv->button);
free_led:
	gpiod_put(priv->led);
	return status;
}

static void my_dev_remove(struct platform_device *pdev)
{
	struct my_dev_data *priv = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	device_remove_file(dev, &dev_attr_led);
	device_remove_file(dev, &dev_attr_button);

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
