# 03_gpioctrl (FreeBSD version)

A style FreeBSD kernel module that can both turn a GPIO pin ON and read from another pin. It will be using the [gpiobus -- GPIO bus system](https://man.freebsd.org/cgi/man.cgi?gpiobus(4)) kernel interface.

This example can be compiled and run on a Raspberry Pi running FreeBSD or on an x86 computer.

Unlike the 01_hello, this kernel module requires building the kernel headers and artifacts for FreeBSD. If you have not already done so, you can build them with the following commands:
```shell
# cd /usr/src
# make -j$(sysctl -n hw.ncpu) buildkernel
```

## Hardware Setup
![Raspberry Pi Wiring](led_button_Steckplatine.png)

## Commands to manage the module

Follow the kernel's log for new lines (recommended to do so in a separate terminal or tmux)
```shell
$ tail -F /var/log/messages
```

Loading the kernel module
```shell
# kldload ./gpioctrl.ko
```

Unloading the kernel module
```shell
# kldunload gpioctrl
```

## Finding the Correct GPIO Numbers
Check your pins at [pinout.xyz](https://www.pinout.xyz)

Using the `devinfo` command to find information regarding the gpio chips on the board:

```shell
$ devinfo -rv | grep gpio
      gpio0 pnpinfo name=gpio@7e200000 compat=brcm,bcm2711-gpio 
        gpiobus0
        gpioc0
        gpio1 pnpinfo name=gpio compat=raspberrypi,firmware-gpio 
          gpiobus1
          gpioc1
      unknown pnpinfo name=gpiomem compat=brcm,bcm2835-gpiomem 
    gpioled0 pnpinfo name=leds compat=gpio-leds 
    gpioregulator0 pnpinfo name=sd_io_1v8_reg compat=regulator-gpio
```

We know that _bcm2711-gpio_ is the main GPIO controller on the Raspberry Pi, so we will use the GPIO with Offset 0.

## Cross Compiling From x86 FreeBSD to AArch64 FreeBSD

It is possible to cross compile the kernel module from an x86 version of FreeBSD to the Raspberry Pi version. Edit your Makefile as follows:

```shell
KMOD=gpioctrl

SRCS=gpioctrl.c device_if.h bus_if.h

CC=clang --target=aarch64-unknown-freebsd

CFLAGS+= -I/usr/src/sys/arm64/include

MACHINE_ARCH=aarch64

MACHINE=arm64

.include <bsd.kmod.mk>
```

The file gpio.ko can then be copied to the Raspberry Pi and loaded into the kernel with kldload command.