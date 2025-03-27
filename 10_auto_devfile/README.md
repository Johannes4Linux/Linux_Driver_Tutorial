# 09_manual_cdev

Unitl now, we have created device files over the shell command `mknod`. In this lecture I will show you how to create the device file from within the kernel module with a little bit of help from udev, the Linux Device Manager.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Explaining the code

So, we have already registered a range of device numbers and we have created a character device. In order to create a device we need to do two additional steps:

- Create a class in `/sys/class`
- Create a device for this class

The device manager udev will see we have created a device and create the device file automatically for us in /dev.

### Initialization

A class is represented as a pointer from the type `struct class`. We add a global variable from this type, we call `my_class`. To create the class, we call the function `class_create(const char *class_name)` which needs a string telling the name of the class as an argument and returns a pointer on success, a NULL pointer on failure. After creating the class `"my_class"`, it will appear under `/sys/class/my_class`.

In order to create a device, we must call the function `device_create`. The arguments are explained below:

~~~
struct device *device_create(
    struct class *my_class, /* Class under which the device should be created */
    struct device *parent, /* Parent device */
    void *drvdata, /* Driverdata for the device */
    const char *fmt, ... /* Name of the device */
);
~~~

The name of a device can be created in a printf like way. E.g.: `"hello%d", 0` will create a device with the name `hello0`. The return value is a pointer to the create device or a NULL pointer on error. As we don't have a parent device, we will set the field to NULL, but if we would create this device for a PCIe device, we could pass the pointer to the PCIe device as a parent device.

If `device_create` returns a not-NULL pointer, we have successfully created a device under `/sys/class/my_class/hello0`. udev will detect the devie and create the corrseponding device file for us which is `/dev/hello0`.

### Cleanup

For cleaning up, the device must be destroyed and the class must be unregistered and removed.

~~~
device_destroy(my_class, dev_nr);
class_unregister(my_class);
class_destroy(my_class);
~~~

### Why use goto

The `goto` statement is somewhat frowned upon in C programming. The reason for this is that you can quickly make a programme confusing with `goto`. However, `goto` is very suitable for hierarchical deinitialisation. Instead of always deinitialising everything in the event of an error, a hierarchical structure of labels is created and depending on how far into the initialisation an error is encountered, the corresponding label is jumped to.

This makes the code much shorter and even clearer. Why don't you try implementing the init function without `goto`? You will see that in the event of an error, you will always execute the same code and the init function will be considerably longer.

## Testing the code

With `udevadm monitor` you can monitor the activity from udev while loading the kernel module. After loading the kernel module, you should see the folder and the device file:

~~~
pi@raspberry:~/Programming/Linux_Driver_Tutorial/10_auto_devfile
$ ls /sys/class/my_class/hello0
dev  power  subsystem  uevent
pi@raspberry:~/Programming/Linux_Driver_Tutorial/10_auto_devfile
$ ls /dev/hello0 -lh
crw------- 1 root root 236, 0 Mar 27 10:29 /dev/hello0
~~~

Reading from the device file should generate a print to the kernel's log.

After removing the module, the class and the device file are gone again.
