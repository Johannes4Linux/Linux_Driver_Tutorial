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
# kldload ./hello.ko
```

Unloading the kernel module
```shell
# kldunload ./hello.ko
```

## Cross Compiling From x86 FreeBSD to AArch64 FreeBSD

It is possible to cross compile the kernel module from an x86 version of FreeBSD to the Raspberry Pi version. Edit your Makefile as follows:

```Make
KMOD=hello
SRCS=hello.c

CC=clang --target=aarch64-unknown-freebsd

CFLAGS+= -I/usr/src/sys/arm64/include

MACHINE_ARCH=aarch64
MACHINE=arm64

.include <bsd.kmod.mk>
```

The file hello.ko can then be copied to the Raspberry Pi and loaded into the kernel with kldload command.
