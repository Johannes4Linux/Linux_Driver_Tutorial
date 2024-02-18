#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#include <linux/poll.h>
#include <linux/wait.h>

#define MYMAJOR 64

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("Kernel Module which use poll to notify an userspace app when GPIO 17 has a rising edge");

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
static int irq_ready = 0;
static wait_queue_head_t waitqueue;

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static irqreturn_t gpio_irq_poll_handler(int irq, void *dev_id) {
	printk("gpio_irq_poll: Interrupt was triggered and ISR was called!\n");
	irq_ready = 1;
	wake_up(&waitqueue);
	return IRQ_HANDLED;
}

/**
 * @brief Poll callback to allow userspace app to poll for button being pressed 
 */
static unsigned int my_poll(struct file *file, poll_table *wait) {
	poll_wait(file, &waitqueue, wait);
	if(irq_ready == 1) {
		irq_ready = 0;
		return POLLIN;
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.poll = my_poll
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("qpio_irq: Loading module... ");

	/* Init qaitqueue */
	init_waitqueue_head(&waitqueue);

	/* Setup the gpio */
	if(gpio_request(17, "rpi-gpio-17")) {
		printk("Error!\nCan not allocate GPIO 17\n");
		return -1;
	}

	/* Set GPIO 17 direction */
	if(gpio_direction_input(17)) {
		printk("Error!\nCan not set GPIO 17 to input!\n");
		gpio_free(17);
		return -1;
	}

	gpio_set_debounce(17, 300);

	/* Setup the interrupt */
	irq_number = gpio_to_irq(17);

	if(request_irq(irq_number, gpio_irq_poll_handler, IRQF_TRIGGER_RISING, "my_gpio_irq_poll", NULL) != 0){
		printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
		gpio_free(17);
		return -1;
	}

	if(register_chrdev(MYMAJOR, "gpio_irq_poll", &fops) < 0) {
		printk("Error!\n Can't register device Number!\n");
		gpio_free(17);
		free_irq(irq_number, NULL);
	}

	printk("Done!\n");
	printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);

	/* Init waitqueue */
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("gpio_irq_poll: Unloading module... ");
	free_irq(irq_number, NULL);
	gpio_free(17);
	unregister_chrdev(MYMAJOR, "gpio_irq_poll");
	printk("Done!\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
