# Linux Driver Tutorial

Here you can find examples for simple Linux Kernel Modules and Linux Drivers.

## Preparation

I used a Raspberry Pi 3 to develop and test my modules and drivers. To compile them, you need to install the Kernel headers on your Pi. On Raspberry Pi OS you can do this with the following command:

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install raspberrypi-kernel-headers
reboot
```
Raspberry Pi OS is only installs the latest kernel headers. So, make sure, you are running the latest kernel.

You also need the build utils (make, gcc, ...) but they come preinstalled on Raspberry Pi OS.

## Content

In this repo you can find examples for:
1. Simple Hello World Kernel Module
2. An improved Hello World Linux Kernel Module
3. An example for using GPIOs in a driver without device tree
4. Some info about Device Files, Device Numbers, Block and Character Devices
5. Create a Character Device in a Driver
6. Linux Kernel log levels
7. Implement open and release functions for character devices
8. Implement read and write functions for character devices
9. Create a Character Device in a Driver manually
10. Automatically create a Device File within the driver

## More Information

For more information about my Linux Driver examples check out my [videos and my playlist](https://www.youtube.com/watch?v=x1Y203vH-Dc&list=PLCGpd0Do5-I3b5TtyqeF1UdyD4C-S-dMa)

## Support me

If you want to support me, you can buy me a coffee [buymeacoffee.com/johannes4linux](https://www.buymeacoffee.com/johannes4linux).
