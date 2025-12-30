# 18_dev_logging

In this lecture you will learn how to add device information when writing to the Linux Kernel's log.

This will only work on Linux Systems with Device Tree Support.

## New logging functions

Until now we only used macros for `printk` which asigns a logging level to it, e.g. `pr_info`. But now we have a proper Linux Device Driver and it would be nice to have some additional device information when writing to the Kernel's log. To achive this, the following device logging macros are available:

|     Name     | String |                 Alias function                  |
|:------------:|:------:|:-----------------------------------------------:|
| KERN_EMERG   | “0”    | dev_emerg()                                     |
| KERN_ALERT   | “1”    | dev_alert()                                     |
| KERN_CRIT    | “2”    | dev_crit()                                      |
| KERN_ERR     | “3”    | dev_err()                                       |
| KERN_WARNING | “4”    | dev_warn()                                      |
| KERN_NOTICE  | “5”    | dev_notice()                                    |
| KERN_INFO    | “6”    | dev_info()                                      |
| KERN_DEBUG   | “7”    | dev_debug() and dev_devel() if DEBUG is defined |
| KERN_DEFAULT | “”     |                                                 |

Besides the format string and arguments, the macros needs a pointer to the device for which this logging message should be generated. So, you have to pass a pointer from the type `struct device` to the macros. E.g. for our platform device:

~~~
dev_info(&pdev->dev, "Remove Function is called!\n");
~~~

## Device Tree overlay

To make it a little bit more interesting, there are two devices defined in the device tree overlay. Both are compatible with my_dev_driver. The first device's name is `my_device`, the second one is `my_dev2`. `my_dev2` also has an address assigned to it which is 0x12345678.

## Testing

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_dev_driver.ko

# Insert the dt overlay
sudo dtoverlay my_overlay.dtbo
# If you check the kernel's log know, you should see the print

# Remove the dt overlay
sudo dtoverlay -R my_overlay
[ 1372.534913] my_dev_driver my_device: Probe Function is called!
[ 1372.534946] my_dev_driver my_device: This is a warning
[ 1372.534959] my_dev_driver my_device: This is an error: 123
[ 1372.539672] my_dev_driver my_dev2@12345678: Probe Function is called!
[ 1372.539706] my_dev_driver my_dev2@12345678: This is a warning
[ 1372.539719] my_dev_driver my_dev2@12345678: This is an error: 123


# Unload the driver
sudo rmmod my_dev_driver
[ 1410.134981] my_dev_driver my_dev2@12345678: Remove Function is called!
[ 1410.135248] my_dev_driver my_device: Remove Function is called!
~~~

You can see that the driver's and device's name is added in front of the line which is printed out to the Kernel's log. If a device has an address, the address is also visible.
