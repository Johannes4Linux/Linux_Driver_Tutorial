# 16_dt_dev_driver

In this lecture you will learn how to implement a Linux Device Driver on the example of a Platform Device. But the general concept is valid for any device, nethertheless if it is an I2C, PCIe, USB, ... device.

This will only work on Linux Systems with Device Tree Support.

## Writing a Linux Device Driver

If you want to write a Linux Device Driver you always have to follow the same steps:

1. Name the compatible devices the driver is responsible for
2. Implement a probe and remove function
3. Bundle the probe, remove function, the compatible devices list and some more driver data in a driver struct
4. Register the driver struct at the OS

## Name compatible devices

The compatible devices must be put in an array of `struct of_device_id`. The list must be terminated with an empty element which is used by the kernel to detect the end of the list. Then each compatible devices gets an entry in the list. For our device, we just have one device the driver should support, so we add an entry for it.

We only assign the `compatbile` field of the struct. The string we put there must match the compatible string we used in our device tree overlay. The struct has more fields, e.g. a data field which would allow us to give each individual device some device specific data, but we don't need it for now.

The macro `MODULE_DEVICE_TABLE` is required for maintaining a list for driver autoloading.

## Implement probe and remove functions

The next step is to implement a probe and a remove function for the device. The probe function is called when a compatible device is added to the system and the remove function when a compatible device is removed.

In the probe function resources for the device should be allocated, e.g. memory, map resources, interrupts, ... In the remove functions these resources are freed again. As allocating resources can fail, the probe function has an `int` return value. If probe returns `0` the device was probbed successfully. Else a negative error code will be returned. The remove function just frees the allocated resources and therefore has no return value.

Both functions have on argument from the type `struct platform_device *`. Over this pointer the properties assigned to the device in the device tree can be accessed.

In our probe and remove functions we just print out one line to the kernel's log to indicate the functions were called.

## Packing the driver struct

Now we can pack the compatible devices, a driver name, the probe and remove function in a `struct platform_driver`.

## Registering the driver

The last remaining step is to register the driver at the operating system. Therefore we can call the function `platform_driver_register` in the init function of the driver. As an argument ``platform_driver_register` needs the pointer to the driver's struct. The function returns `0` on success, a negative error code else.

After registering the driver the kernel knows when a compatible device appears to call the probe function mentioned in the driver's struct.

To unregister the driver, `platform_driver_unregister` is called in the exit function.

If the init and exit function just (un)register the driver, the macro `module_platform_driver` can be used to create the init and exit functions automatically. Only use `platform_driver_register` if you need to allocate more resources (e.g. global device list, range of device numbers, ...) in the init function.

## Testing

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_dev_driver.ko
# If you check the kernel's log know, you should see the print
# my_dev - Init function is called

# Insert the dt overlay
sudo dtoverlay my_overlay.dtbo
# If you check the kernel's log know, you should see the print
# my_dev - Probe Function is called!

# Remove the dt overlay
sudo dtoverlay -R my_overlay
# If you check the kernel's log know, you should see the print
# my_dev - Remove Function is called!

# Unload the driver
sudo rmmod my_dev_driver
# If you check the kernel's log know, you should see the print
# my_dev - Exit Function is called!
~~~
