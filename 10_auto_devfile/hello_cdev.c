#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static dev_t dev_nr;
static struct cdev my_cdev;
static struct class *my_class;

static ssize_t my_read(struct file *f, char __user *u, size_t l, loff_t *o)
{
	pr_info("hello_cdev - Read is called\n");
	return 0;
}


static struct file_operations fops = {
	.read = my_read
};

static int __init my_init(void)
{
	int status;
#ifdef STATIC_DEVNR
	dev_nr = STATIC_DEVNR;
	status = register_chrdev_region(dev_nr, MINORMASK + 1, "hello_cdev");
#else
	status = alloc_chrdev_region(&dev_nr, 0, MINORMASK + 1, "hello_cdev");
#endif
	if (status) {
		pr_err("hello_cdev - Error reserving the region of device numbers\n");
		return status;
	}

	cdev_init(&my_cdev, &fops);
	my_cdev.owner = THIS_MODULE;

	status = cdev_add(&my_cdev, dev_nr, MINORMASK + 1);
	if (status) {
		pr_err("hello_cdev - Error adding cdev\n");
		goto free_devnr;
	}

	pr_info("hello_cdev - Registered a character device for Major %d starting with Minor %d\n", MAJOR(dev_nr), MINOR(dev_nr));

	my_class = class_create("my_class");
	if (!my_class) {
		pr_err("hello_cdev - Could not create class my_class\n");
		status = ENOMEM;
		goto delete_cdev;
	}

	if (!device_create(my_class, NULL, dev_nr, NULL, "hello%d", 0)) {
		pr_err("hello_cdev - Could not create device hello0\n");
		status = ENOMEM;
		goto delete_class;
	}

	pr_info("hello_cdev - Created device under /sys/class/my_class/hello0\n");

	return 0;

delete_class:
	class_unregister(my_class);
	class_destroy(my_class);
delete_cdev:
	cdev_del(&my_cdev);
free_devnr:
	unregister_chrdev_region(dev_nr, MINORMASK + 1);
	return status;
}

static void __exit my_exit(void)
{
	device_destroy(my_class, dev_nr);
	class_unregister(my_class);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_nr, MINORMASK + 1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("A sample driver for manually registering a character device");
