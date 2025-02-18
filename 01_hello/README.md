# 01_hello (FreeBSD version)

A simple hello world style FreeBSD kernel module.

This example can be compiled and run on a Raspberry Pi running FreeBSD or on an x86 computer.

## Commands to manage the module

Follow the kernel's log for new lines
```shell
$ tail -F /var/log/messages
```

Loading the kernel module
```shell
$ sudo kldload ./hello.ko
```

Unloading the kernel module
```shell
$ sudo kldunload ./hello.ko
```

