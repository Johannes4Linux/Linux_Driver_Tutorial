# 01_hello

A simple hello World Linux Kernel Module.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## The Hello World Kernel Module

For a Kernel Module two events are important and that is when the module is loaded into the kernel and when it is removed from the kernel. With the macros `module_init` and `module_exit` we can set the functions which should be executed on these events.

The prototype of the function which is called when the module is loaded into the kernel looks as follows:

~~~
int my_init(void)
~~~

The function has a return value from the type `int`. If the module was loaded successfully into the kernel, the function returns `0`. If an error occurs during the initialization, the function returns a negative error code.

The init function doesn't have any arguments which are passed to it.

The prototype of the function which is called when the module is removed from the kernel looks as follows:

~~~
void my_exit(void)
~~~

It doesn't have any return value or arguments in it. Its purpose is to clean up everything done in the init function (e.g. freeing resources like memory, interrupts, ...).

In both functions `printk` is used. This function looks very similar to `printf` which writes to the Standard Output. But for a kernel module, there is no standard output. The kernel's log is the only thing available to print status out to. So, `printk` writes to the kernel's log.

To finish the module, one last important Meta-Data is missing. This Meta-Data is the license of the module which can be set with the following macro:

~~~
MODULE_LICENSE("GPL");
~~~

So, we are using the GPL license, which is a copy-left free and open source license. A license is required for the kernel module as some GNU/Linux distributions only allow to load open source drivers. If we would set the license to `proprietary` some distributions could refuse to load the driver.

## Makefile for building the module

The Linux kernel is build over a Makefile. And for building a kernel module we also must write a Makefile. 

First, we must add our kernel module to a list of object files which should be build as standalone kernel modules:

~~~
obj-m += hello.o
~~~

The compilation from `hello.c` to an object file `hello.o` is done automatically by the kernel header's Makefile. 

The next step is to declare two targets `all` and `clean`. For both targets we are calling the Makefile which is part of the kernel headers located at `/lib/modules/$(uname -r)/build`. By setting the `M` variable to our current path, we tell the kernel header's Makefile to look in our current path for kernel modules to build. 

The only difference between the two targets are the targets they are calling from the kernel header's Makefile. `all` calls the `modules` target, which will build the kernel modules. `clean` calls the `clean` target which will delete all files created during the compilation.

## Commands to manage the module

- Following the kernel's log for new lines
  ~~~
  sudo dmesg -W
  ~~~
- Inserting the module:
  ~~~
  sudo insmod hello.ko
  ~~~
- Removing the module:
  ~~~
  sudo rmmod hello.ko
  ~~~

