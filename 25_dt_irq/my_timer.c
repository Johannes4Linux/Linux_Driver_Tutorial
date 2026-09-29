#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>

/* Name all the compatbile devices */
static const struct of_device_id my_timer_ids[] = {
	{ .compatible = "arm,my_timer"},
	{} /* Empty Element -> end of the list */
};
MODULE_DEVICE_TABLE(of, my_timer_ids);

static struct resource *res;
static u32 __iomem *hwregs;

#define LOAD_REG	(0x00 / 4)
#define VALUE_REG	(0x04 / 4)
#define CTRL_REG	(0x08 / 4)
#define IRQCNTL_REG	(0x0C / 4)
#define RAWIRQ_REG	(0x10 / 4)

static irqreturn_t my_timer_isr(int irq, void *data)
{
	struct device *dev = (struct device *) data;

	if (*(hwregs + RAWIRQ_REG))
		*(hwregs + IRQCNTL_REG) = 0x1;

	dev_info(dev, "IRQ %d occurred!\n", irq);
	return IRQ_HANDLED;
}

/* Implement a probe and a remove function */
static int my_timer_probe(struct platform_device *pdev)
{
	int irq, status;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Error could not get resource\n");
		return -ENODEV;
	}

	dev_info(&pdev->dev, "Resource at %llx (%lld Bytes in size)\n", res->start, resource_size(res));

	hwregs = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!hwregs) {
		dev_err(&pdev->dev, "Error could not map resource\n");
		return -ENODEV;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "Error could not get IRQ\n");
		return -ENODEV;
	}

	status = devm_request_irq(&pdev->dev, irq, my_timer_isr, 0, "RPi ARM Timer", &pdev->dev);
	if (status) {
		dev_err(&pdev->dev, "Error requesting IRQ %d\n", irq);
		return status;
	}


	/* Set Load register */
	*(hwregs + LOAD_REG) = 0x80000;

	/* Enable the timer */
	*(hwregs + CTRL_REG) = 0xaa;


	return 0;
}

static void my_timer_remove(struct platform_device *pdev)
{
	/* Disable the timer */
	*(hwregs + CTRL_REG) = 0x3e0000;
}

/* Bundle compatible devices, probe and remove in driver's struct */
static struct platform_driver my_timer_driver = {
	.probe = my_timer_probe,
	.remove = my_timer_remove,
	.driver = {
		.name = "my_timer_driver",
		.of_match_table = my_timer_ids,
	}
};

/* Register the driver at the OS */
module_platform_driver(my_timer_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("Driver for the RPi ARM Timer");
