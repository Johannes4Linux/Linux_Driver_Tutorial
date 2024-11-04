# 02_better_hello

An improved hello World Linux Kernel Module.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Changes

We have added some Metadata for the Kernel Module, declared the functions  `my_init` and `my_exit` static to limit visibility and linkage and added the `__init` and `__exit` macros to increase readability.

## Commands for managing kernel modules

- Showing the Kernel module's metadata:
  ~~~
  # With path and filename
  modinfo ./hello.ko
  # For modules shipped with our kernel
  modinfo industrialio
  ~~~
- Listing all the loaded modules
  ~~~
  lsmod
  # Search for our one
  lsmod | grep hello
  ~~~
- Showing the Kernel's log
  ~~~
  # The whole kernel's log
  sudo dmesg
  # Just the last 5 lines of the log
  sudo dmesg | tail -n 5
  # Print out kernel's log and follow for new lines
  sudo dmesg -w
  ~~~
- Loading module with its dependencies (just for modules shipped with our installed kernel)
  ~~~
  sudo modprobe industrialio
  ~~~
- Removing the module:
  ~~~
  # with path and filename
  sudo rmmod ./hello.ko
  # with module's name as shown in lsmod
  sudo rmmod hello
  ~~~




