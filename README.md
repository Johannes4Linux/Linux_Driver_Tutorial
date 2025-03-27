# FreeBSD Driver Tutorial

This is a fork of [Johannes4Linux](https://github.com/Johannes4Linux) tutorial series [Linux Kernel Modules and Linux Drivers](https://github.com/Johannes4Linux/Linux_Driver_Tutorial), but instead for creating driver modules for FreeBSD rather than linux. The goal is to recreate the lessons that Johannes created for Linux, but on FreeBSD.

I am writing more detailed explanations of these kernel modules with step by step in commentary on my blog: [jholloway.dev](https://jholloway.dev). Please check there for more information regarding my journey in FreeBSD driver development.

## Preparation

I used a Raspberry Pi 4 Rev 4 with FreeBSD 14.2 installed to develop and test my modules and drivers. To compile them, you need to ensure that you have installed the FreeBSD source tree to /usr/src. If you have not, follow these steps on your Raspberry Pi installation of FreeBSD. 

Note: the _#_ indicates that the commands are be run as root. FreeBSD does not come with sudo installed by default, and instead to run the commands as a non-root user you will need to install either [sudo](https://www.freshports.org/security/sudo/) or [doas](https://www.freshports.org/security/doas/).

```bash
# git clone --branch releng/14.2 https://git.FreeBSD.org/src.git /usr/src
```
Some kernel modules will require the kernel headers and artifacts from the build process of building the kernel. To build the kernel on your Raspberry Pi clone the src to /usr/src and run the following command:

```bash
# cd /usr/src
# make -j$(sysctl -n hw.ncpu) buildkernel
```
This will build the toolchain, header files, artifacts, and a generic FreeBSD kernel for your Pi in the directory `/usr/obj/usr/src/arm64.aarch64/sys/GENERIC`.This will create the required kernel headers and artifacts  For modules which require kernel artifacts, you will need to include this folder in your Makefile. This process can take time with the limited hardware that is on the Pi. For that reason, it can be faster to cross compile from a more powerful laptop or desktop and copy the kernel modules to your Pi.


### Cross Compiling to the Raspberry Pi

In order to cross compile you will need to first make a kernel for the Raspberry Pi. Official FreeBSD images for the Pi use a generic ARM64 kernel, so we will compile a ARM64 generic kernel as well.

```bash
# git clone --branch releng/14.2 https://git.FreeBSD.org/src.git /usr/src
# -j$(sysctl -n hw.ncpu) TARGET=arm64 TARGET_ARCH=aarch64 KERNCONF=GENERIC buildkernel

```

The MakeFile for each module will need to be updated to include the following lines specifying the architecture:

```make
CC=clang --target=aarch64-unknown-freebsd
MACHINE_ARCH=aarch64
MACHINE=arm64
```

For more information on getting FreeBSD setup on a Raspberry Pi, please see the post [First Kernel Modules](https://jholloway.dev/posts/first-kernel-modules/)

## Content

In this repo you can find examples for:
1. Simple Hello World kernel Module remade for FreeBSD
2. A better Hello World kernel module remade for FreeBSD that provides metadata for the kernel module.
3. An example for using GPIOs to read from a pin or turn on a pin in a kernel module.

## More Information

For more information about Johannes4Linux tutorial series please see his [tutorial playlist](https://www.youtube.com/watch?v=x1Y203vH-Dc&list=PLCGpd0Do5-I3b5TtyqeF1UdyD4C-S-dMa)
I also recommend supporting Johannes4Linux and buying him a coffee [buymeacoffee.com/johannes4linux](https://www.buymeacoffee.com/johannes4linux).

All credit on this series goes to him as he was the one who got the ball rolling. This branch is merely a recreation of the work he has already done.
