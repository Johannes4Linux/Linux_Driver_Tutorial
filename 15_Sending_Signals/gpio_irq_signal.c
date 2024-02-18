#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/ioctl.h>

#define MYMAJOR 64

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("Kernel Module which sends a signal to an userspace app when GPIO 17 has a rising edge");

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;

/** Global variables and defines for userspace app registration */
#define REGISTER_UAPP _IO('R', 'g')
static struct task_struct *task = NULL;

/* define for Signal sending */
#define SIGNR 44

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static irqreturn_t gpio_irq_signal_handler(int irq, void *dev_id)
{
	struct siginfo info;
	printk("gpio_irq_signal: Interrupt was triggered and ISR was called!\n");

	if(task != NULL) {
		memset(&info, 0, sizeof(info));
		info.si_signo = SIGNR;
		info.si_code = SI_QUEUE;

		/* Send the signal */
		if(send_sig_info(SIGNR, (struct kernel_siginfo *) &info, task) < 0)
			printk("gpio_irq_signal: Error sending signal\n");
	}
	return IRQ_HANDLED;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int my_close(struct inode *device_file, struct file *instance) {
	if(task != NULL)
		task = NULL;
	return 0;
}

/**
 * @brief IOCTL for registering the Userspace app to the kernel module
 */
static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
	if(cmd == REGISTER_UAPP) {
		task = get_current();
		printk("gpio_irq_signal: Userspace app with PID %d is registered\n", task->pid);
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.release = my_close,
	.unlocked_ioctl = my_ioctl,
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("qpio_irq: Loading module... ");

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

	if(request_irq(irq_number, gpio_irq_signal_handler, IRQF_TRIGGER_RISING, "my_gpio_irq_signal", NULL) != 0){
		printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
		gpio_free(17);
		return -1;
	}

	if(register_chrdev(MYMAJOR, "gpio_irq_signal", &fops) < 0) {
		printk("Error!\n Can't register device Number!\n");
		gpio_free(17);
		free_irq(irq_number, NULL);
	}

	printk("Done!\n");
	printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("gpio_irq_signal: Unloading module... ");
	free_irq(irq_number, NULL);
	gpio_free(17);
	unregister_chrdev(MYMAJOR, "gpio_irq_signal");
	printk("Done!\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
