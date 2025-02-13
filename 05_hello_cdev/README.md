# 05_hello_cdev

A demonstration for how to create Character Device Files in a Linux Driver or Kernel Module

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Explaining the code

In order to create an interface which can be accessed over a Device File, the Driver has to do the following:

- Allocate a Device Number
- Creating a Character Device linked to the Device Number

Luckily Linux offers one function to perform both steps:

~~~
int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops);
~~~

*major* is the Major Device Number to use. If set to 0, register_chrdev will search for the next free Device Number and use it. register_chrdev will allocate 256 Device Numbers, all with the same Major Device Number. The *name* string is a label which will appear in /proc/devices. The last argument is a pointer to a file_operations struct, which contains all the file operations (read, write, open, close, mmap, ...) which should be supported by the Character Device.

If the return value is negative an error occured during the registration of the Character Device or the allocation of the Device Number (e.g. Device Number already in use). If *major* is set to 0, it will return the Major Device Number on success, if *major* is a number, it will return 0 on success.

In the exit function we have to delete the Character Device and free the Device Number. This can be done with:

~~~
void unregister_chrdev(unsigned int major, const char *name);
~~~

## Testing the code

After loading the kernel module, you should get a print in the kernel's log:

~~~
[ 3038.299101] hello_cdev - Major Device Number: 236
~~~

The Major Device Number can be different on your system. If you take a look at */proc/devices* you can also find out the Device Number:

~~~
grep hello_cdev /proc/devices
236 hello_cdev
~~~

Now, let's create a Device File with the corresponding Device Number:

~~~
sudo mknod /dev/hello0 c 236 0
~~~

If we read from this file, we should see te following print in the kernels log:

~~~
sudo cat /dev/hello0
sudo dmesg | tail -n 1
[ 3117.149854] hello_cdev - Read is called
~~~

To test, that *register_chrdev* really allocated the whole range of Device Numbers starting with 236, let's create a second Device File:

~~~
sudo mknod /dev/hello128 c 236 128
~~~

If we read from that device file, we can also see the print in the kernel's log:

~~~
sudo cat /dev/hello128
sudo dmesg | tail -n 1
[ 3120.254910] hello_cdev - Read is called
~~~

