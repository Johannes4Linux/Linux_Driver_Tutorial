#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>

static char text[64];

static ssize_t my_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

	pr_info("hello_miscdev - Read is called, we want to read %ld bytes, but actually only copying %d bytes. The offset is %lld\n", len, to_copy, *off);

	if (*off >= sizeof(text))
		return 0;

	not_copied = copy_to_user(user_buf, &text[*off], to_copy);
	delta = to_copy - not_copied;
	if (not_copied) 
		pr_warn("hello_miscdev - Could only copy %d bytes\n", delta);

	*off += delta;

	return delta;
}

static ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

	pr_info("hello_miscdev - Write is called, we want to write %ld bytes, but actually only copying %d bytes. The offset is %lld\n", len, to_copy, *off);

	if (*off >= sizeof(text))
		return 0;

	not_copied = copy_from_user(&text[*off], user_buf, to_copy);
	delta = to_copy - not_copied;
	if (not_copied) 
		pr_warn("hello_miscdev - Could only copy %d bytes\n", delta);

	*off += delta;
	return delta;
}

static struct file_operations fops = {
	.read = my_read,
	.write = my_write
};

static struct miscdevice my_miscdev = {
	.fops = &fops,
	.name = "my_miscdev",
	.minor = MISC_DYNAMIC_MINOR,
};

static int __init my_init(void)
{
	int status = misc_register(&my_miscdev);
	if (status) {
		pr_err("hello_miscdev - Error registering my_miscdev\n");
		return status;
	}

	pr_info("hello_miscdev - /dev/my_miscdev created with minor dev nr: %d\n", my_miscdev.minor);

	return 0;
}

static void __exit my_exit(void)
{
	misc_deregister(&my_miscdev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("A sample driver for registering a character device");
