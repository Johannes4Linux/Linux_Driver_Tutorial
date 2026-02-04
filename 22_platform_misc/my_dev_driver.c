#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/miscdevice.h>

/* Name all the compatbile devices */
static const struct of_device_id my_dev_ids[] = {
	{ .compatible = "brightlight,mydev"},
	{} /* Empty Element -> end of the list */
};
MODULE_DEVICE_TABLE(of, my_dev_ids);

struct my_dev_data {
	struct gpio_desc *led;
	struct gpio_desc *button;
	struct miscdevice mdev;
};

static int devcnt = 0;

static inline struct my_dev_data *to_my_dev_data(struct file *filp)
{
	struct miscdevice *m = filp->private_data;

	return container_of(m, struct my_dev_data, mdev);

}

static ssize_t my_dev_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	struct my_dev_data *priv = to_my_dev_data(filp);
	char text[32];
	int status;
	int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

	if (*off >= sizeof(text))
		return 0;

	status = gpiod_get_value(priv->button);
	snprintf(text, sizeof(text), "Button is %spressed\n", status ? "" : "not ");

	not_copied = copy_to_user(user_buf, &text[*off], to_copy);
	delta = to_copy - not_copied;

	*off += delta;

	return delta;
}

static ssize_t my_dev_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	struct my_dev_data *priv = to_my_dev_data(filp);
	char text[32];
	int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

	if (*off >= sizeof(text))
		return 0;

	not_copied = copy_from_user(&text[*off], user_buf, to_copy);
	delta = to_copy - not_copied;

	if (text[0] == '0')
		gpiod_set_value(priv->led, 0);
	if (text[0] == '1')
		gpiod_set_value(priv->led, 1);

	*off += delta;
	return delta;
}

static struct file_operations fops = {
	.read = my_dev_read,
	.write = my_dev_write
};


/* Implement a probe and a remove function */
static int my_dev_probe(struct platform_device *pdev)
{
	struct my_dev_data *priv;
	int status;
	char name[16];
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

	priv->mdev.fops = &fops;
	priv->mdev.minor = MISC_DYNAMIC_MINOR;
	sprintf(name, "my_dev%d", devcnt++);
	priv->mdev.name = name;

	status = misc_register(&priv->mdev);
	if (status) {
		dev_err(dev, "Could not register miscdevice\n");
		goto free_button;
	}
	platform_set_drvdata(pdev, priv);

	return 0;

free_button:
	gpiod_put(priv->button);
free_led:
	gpiod_put(priv->led);
	return status;
}

static void my_dev_remove(struct platform_device *pdev)
{
	struct my_dev_data *priv = platform_get_drvdata(pdev);

	devcnt--;
	misc_deregister(&priv->mdev);
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
