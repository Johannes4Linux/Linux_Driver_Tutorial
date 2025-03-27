# 01_hello (FreeBSD version)

A simple hello world style FreeBSD kernel module.

This example can be compiled and run on a Raspberry Pi running FreeBSD or on an x86 computer.

## Commands to manage the module

Follow the kernel's log for new lines (recommended to do so in a separate terminal or tmux)
```shell
$ tail -F /var/log/messages
```

Loading the kernel module
```shell
# kldload ./better_hello.ko
```

To get the metadata from the sysctrl use the following commands:

```shell
$ sysctl kern.better_hello
kern.better_hello.version: 1.0
kern.better_hello.license: BSD
kern.better_hello.description: A simple Hello World kernel module
kern.better_hello.author: John Holloway
```

To get more specific information, you can enter the child node in the sysctl command.
```shell
$ sysctl kern.better_hello.author
kern.better_hello.author: John Holloway
```

Unloading the kernel module
```shell
# kldunload better_hello
```

## Cross Compiling From x86 FreeBSD to AArch64 FreeBSD

It is possible to cross compile the kernel module from an x86 version of FreeBSD to the Raspberry Pi version. Edit your Makefile as follows:

```Make
KMOD=better_hello
SRCS=better_hello.c

CC=clang --target=aarch64-unknown-freebsd
CFLAGS+= -I/usr/src/sys/arm64/include

MACHINE_ARCH=aarch64
MACHINE=arm64

.include <bsd.kmod.mk>
```

The file hello.ko can then be copied to the Raspberry Pi and loaded into the kernel with kldload command.
