# 12_ioctl_simple

We already learned how to implement the open, close, read and write systemcalls for a character device in a Linux Kernel. For a lot of devices this is sufficient. But some devices need functionality beyond reading and writing. They need ways to configure a device, or to read back the configuration or to perform a reset, ... In these cases ioctl can be used to provide this functionality.

## ioctl in the driver

For bringing the ioctl functionality to our driver, we must create a new function with the following prototype:

~~~
static long int my_ioctl(struct file *f, unsigned int cmd, unsigned long args)
~~~

The first argument is a pointer to our currently opened file. `ioctl` is using a `cmd` to detect which operation should be performed. To the command additional arguments can be passed over the `args` pointer.

`ioctl` should return a zero on success and a negative error code elses.

We need to tell the driver `ioctl` is available by setting the corresponding callback function pointer in the `file_operations` struct:

~~~
static struct file_operations fops = {
	.unlocked_ioctl = my_ioctl,
};
~~~

# ioctl in userspace

In a simple userspace program, we need to include `sys/ioctl.h` to be able to use `ioctl`. Then we can just open the device file and execute `ioctl`. 

The first argument is the file descriptor, the second one the command we want to invoke and the last one a pointer to the arguments we want to pass.

The return value indicates, if the `ioctl` worked correctly, or if an error occured.

## Testing the code

Just load the kernel module with `insmod` and then run the compiled userspace program. You will see prints in the kernel's log telling you, that the ioctl function was triggered.
