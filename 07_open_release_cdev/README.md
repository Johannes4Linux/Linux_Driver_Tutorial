# 06_hello_cdev

A demonstration for how to implement the open and release function of a Character Device Files in a Linux Driver or Kernel Module

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Explaining the code

In the `struct file_operations fops` we can set callback functions for various syscalls, like `open`, `read`, `write`, `close`, ... For our character device we have overloaded the `open` function which will be called, when we call the `open` functions for the device file linked to the character device over the device number from userspace and the `release` function, which will be called when we call `close` from userspace.

Here are the prototypes of the callback functions we want to implement:

~~~
int my_open(struct inode *inode, struct file *filp);
int my_release(struct inode *inode, struct file *filp);
~~~

Both functions return 0 on success, else a negative error code. The arguments are also identical. Here is an explanation of the arguments and also some important fields:

- `struct inode *inode`: Kernel representation of a file. We can get the Major and Minor Device numbers from it
- `struct file *filp`: Represents the opened file. Important fields are:
    - `f_mode`: File permissions, e.g. read, write, ...
    - `f_ops`: Pointer to the file operations associated with this file
    - `f_pos`: Current position in file (0 on open)
    - `f_flags`: Flags set by the userspace `open` function, e.g. `O_RDONLY`, `O_RDWR`, ...

The major and minor device number can be read out the `inode`with the function `imajor` and `iminor`.

The file operations are bundled in the `fops` struct. 

## Testing the code

We need a small userspace application to test the driver. You can pass the file to open to the program. Compile it with gcc:


~~~
gcc test.c -o cdev_test
~~~

Load the Kernel Module, check for the device number and create some device files:

~~~
sudo insmod hello_cdev.ko
sudo mknod /dev/hello0 c 236 0
sudo mknod /dev/hello11 c 236 11
~~~

Now, lets run the test application:

~~~
sudo ./cdev_test /dev/hello0
~~~

You should see the prints in the kernel logs. The Minor Device Number should be zero. The `f_mode` and `f_flags` differ on every open, as we are using different flags.

When using the test application with the other device file, the Minor Device Number should be 11 instead of 0.

~~~
sudo ./cdev_test /dev/hello11
~~~

