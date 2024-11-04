# 01_hello

A simple hello World Linux Kernel Module.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

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

