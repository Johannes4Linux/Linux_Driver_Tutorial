# 09_manual_cdev

Unitl now, we have used the function `register_chrdev` to reserve a range of device numbers, create a character device and bind the device number range to it. In this lesson, you will learn how to manually do these steps. You will need this later, when we are also creating a device fle from the Linux Kernel Module.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Explaining the code

First, let's recap how a device number looks like. A device number is split into the Major device number and the Minor device number.

## Reserving a range of device numbers

A range of device numbers can be reserved with a static device number to start with or by dynamically allocating a free range. Let's take a look at both approaches.

#### Using a static device number

For reserving a `count` device numbers starting with the device number `dev_nr` the following function can be used:

~~~
int register_chrdev_region(dev_t dev_nr, unsigned int count, char *name);
~~~

`name` will appear in `/proc/devices` as the name associated with the Major Device number.

#### Using a static device number

For reserving a range of `count` device numbers starting with Minor Device Number `firstminor` and using a free range of device numbers, the following function can be used:

~~~
int alloc_chrdev_region(dev_t *dev_nr, unsigned int firstminor,  unsigned int count, char *name);
~~~

The first argument is a pointer at which the allocated device number will be stored.

Both functions return 0 on success or an error code else.

## Creating the character device

A character device is represented with an object from the type `struct cdev`. To use this, `linux/cdev.h` must be included. In this kernel module a variable from this type is declared as a global variable.

~~~
static struct cdev my_cdev;
~~~

This object will be initialized in the init function. Therefore the function `cdev_init` can be used. Also, the owner of the object will be set to the current module:

~~~
cdev_init(&my_cdev, &fops);
my_cdev.owner = THIS_MODULE;
~~~

The first argument of `cdev_init` is a pointer to the cdev. The second one a pointer to a `file_operaions` struct containing all the file operations which are supported by the cdev. The function has no return value.

To make the character device available on the system, it must be added with `cdev_add`:

~~~
int cdev_add(struct cdev *my_cdev, dev_t dev_nr, unsigned int count);
~~~

The first argument of `cdev_add` is a pointer to the cdev. The second one is the device number, the character device is bind to. The last argument tells to how many device numbers are managed by the cdev. This function returns zero on success, a negative error code else.

## Cleanup

For cleaning up, the cdev must be deleted and the device number must be freed. This can be done with the following functions.

~~~
void cdev_del(struct cdev *my_cdev);
void unregister_chrdev_region(dev_t from, unsigned int count);
~~~

## Testing the code

The module should exactly work like [05_hello_cdev](../05_hello_cdev/README.md).
