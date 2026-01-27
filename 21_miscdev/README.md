# Miscellaneous Devices

A demonstration for how to easily create a device file with a miscellaneous device (miscdev) in a Linux Driver or Kernel Module

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Why should I use it?

The Miscellaneous device API offers a very easy way to gernerate a device file. There are fewer steps required compared to a traditional device file. So, if you have a simple device and need a device file for it, a miscellaneous device can help here.

## Explaining the code

The example code will create a miscellaneous device with the name `my_miscdev` and implement a read and write callback to read and write a global string defined in the driver.

First, the read and write callback functions are implemented and then bundled in a `struct file_operations`.

Then we create our miscellaneous device `my_miscdev` and assign the folliwing fields:

- `.fops` is initialized with a pointer to the supported file operations
- `.name` is the name of the miscellaneous device and will also be used as the name of the device file
- `.minor` sets the minor device number for the miscellaneous device. We could assign a fixed number here (e.g. 12) or let the system pick a free minor device number with the macro `MISC_DYNAMIC_MINOR`.

In the init function we register the miscellaneous device with the function `misc_register`. The function takes a pointer to the miscdev as an argument and returns 0 on success and a negative error code else.

Over the field `my_miscdev.minor` we can access the assigned minor device number.

In the exit function we free the miscdev with `misc_deregister` which also takes a pointer to the miscdev as an argument.

## Testing the code

After loading the kernel module, you should get a print in the kernel's log:

~~~
[ 3038.299101] hello_miscdev - /dev/my_miscdev created with minor dev nr: 121
~~~

So, the my_miscdev has minor device number 121. The number can differ on your system as it was assigned dynamically.

In `/dev` we should be able to find our device file:

~~~
ls -l /dev/my_miscdev 
crw------- 1 root root 10, 121 Jan 27 20:27 /dev/my_miscdev
~~~

All miscellaneous devices are sharing major device number 10. This is one difference compared to the normal character devices where you can assign the major and the minor device number. So, all miscellaneous devices must share the `(1 << 20) -1` available minor device numbers.

Now, let's test the driver by writing and reading from the device file:

~~~
echo "Hello World!" | sudo tee /dev/my_miscdev
~~~

After writing to the file, we see that the write callback function was executed once. To read back the data, run

~~~
sudo cat /dev/my_miscdev
Hello World!
~~~

So, reading and writing to the device file works.

## Summary

Miscellaneous devices offers an easy way to create a device file within your driver. It takes fewer lines of code but with the limitation of the fixed major device number. But for simple and easy devices this is not a problem.

## Resources

[LinuxVox article](https://linuxvox.com/blog/what-is-the-difference-between-misc-drivers-and-char-drivers/#what-are-miscellaneous-misc-drivers)
