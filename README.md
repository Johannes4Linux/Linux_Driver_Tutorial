# FreeBSD Driver Tutorial

This is a fork of [Johannes4Linux](https://github.com/Johannes4Linux) tutorial series [Linux Kernel Modules and Linux Drivers](https://github.com/Johannes4Linux/Linux_Driver_Tutorial), but instead for creating driver modules for FreeBSD rather than linux. The goal is to recreate the lessons that Johannes created for Linux, but on FreeBSD.

## Preparation

I used a Raspberry Pi 4 Rev 4 with FreeBSD 14.2 installed to develop and test my modules and drivers. To compile them, you need to insure that you have installed the FreeBSD source tree to /usr/src. If you have not, follow these steps on your Raspberry Pi installation of FreeBSD. 

```bash
$ sudo git clone --branch releng/14.2 https://git.FreeBSD.org/src.git /usr/src
```
For more information on getting FreeBSD setup on a Raspberry Pi, please see the post [First Kernel Modules](https://jholloway.dev/posts/first-kernel-modules/)

## Content

In this repo you can find examples for:
1. Simple Hello World Kernel Module remade for FreeBSD

## More Information

For more information about Johannes4Linux tutorial series please see his [tutorial playlist](https://www.youtube.com/watch?v=x1Y203vH-Dc&list=PLCGpd0Do5-I3b5TtyqeF1UdyD4C-S-dMa)
I also recommend supporting Johannes4Linux and buying him a coffee [buymeacoffee.com/johannes4linux](https://www.buymeacoffee.com/johannes4linux).

All credit on this series goes to him as he was the one who got the ball rolling. This branch is merely a recreation of the work he has already done.
