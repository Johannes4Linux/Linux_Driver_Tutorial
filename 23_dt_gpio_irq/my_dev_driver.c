#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

/* Name all the compatbile devices */
static const struct of_device_id my_dev_ids[] = {
	{ .compatible = "brightlight,mydev"},
	{} /* Empty Element -> end of the list */
};
MODULE_DEVICE_TABLE(of, my_dev_ids);

static irqreturn_t my_dev_isr(int irq, void *data)
{
	struct platform_device *pdev = (struct platform_device *) data;

	dev_info(&pdev->dev, "IRQ %d was triggered!\n", irq);
	return IRQ_HANDLED;
}

/* Implement a probe and a remove function */
static int my_dev_probe(struct platform_device *pdev)
{
	int status, irq_nr;
	struct device *dev = &pdev->dev;

	dev_info(dev, "my_dev - Probe Function is called!\n");

	irq_nr = platform_get_irq(pdev, 0);
	if (irq_nr < 0) {
		dev_err(dev, "my_dev - Error reading interrupt property\n");
		return irq_nr;
	}

	dev_info(dev, "my_dev - Got IRQ Number %d\n", irq_nr);

	status = devm_request_irq(dev, irq_nr, my_dev_isr, 0, "my_dev_irq", pdev);
	if (status) {
		dev_err(dev, "my_dev - Error requesting interrupt\n");
		return status;
	}

	return 0;
}

static void my_dev_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	dev_info(dev, "my_dev - Remove Function is called!\n");
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
MODULE_DESCRIPTION("DT Driver for my device using a GPIO interrupt");
