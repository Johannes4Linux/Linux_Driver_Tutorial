# 20_dt_dev_data

In this lecture you will learn how to allocate and use device data.

This example can be compiled and run on a Raspberry Pi. The driver should also work on other ARM based platforms but the device tree overlay must be adopted.

## Hardware setup

![TODO: Draw Fritzing of HW Setting]()

One LED is connected to GPIO21, and a second one to GPIO16.

## Problems with the last driver

The driver from the last lecture has a problem. It can only handle one instance of the device, because it is using global pointers for the GPIO descriptors. If we would add two compatible devices, the probe function will be executed twice and the global pointers will be overwritten by the second device.

To work around this issue, we must allocate data per device and bind it to the driver.

## Defining device data

The first thing here is to define a `struct` containing all the necessary device data. For this driver, the device data is just two pointers to GPIO descriptors. One for the LED and one for the button:

~~~
struct my_dev_data {
	struct gpio_desc *led;
	struct gpio_desc *button;
};
~~~

## Allocating device data

In the probe function we must allocate data for the device. We could use `kmalloc` for that, but then we are responsible to free the memory in an error case or when removing the device. Luckily, the Linux Kernel offers device managed functions for memory allocation.

With `devm_kzalloc` we can allocate memory which is bind to the device and managed and initialize it with zeros.. This means the memory will be freed by the kernel automatically when it is no longer needed. The first argument of `devm_kzalloc` is a pointer to the the `struct device` for which the memory should be allocated. The other two arguments are the same as for `kzalloc`.

After allocating the memory, we can use the device data.

## Setting the device data

In order to have the device data available for the device, we need to call the function `platform_set_drvdata`. The first argument is a pointer to the platform device for which we want to set the data and the second one is the data to be set.

In the probe function we retrieve the data by using the `platform_get_drvdata` function which returns a pointer to the device data.

## Modified overlay

In the device tree overlay there is a second device which is compatible with our driver to test if the driver can handle both devices. When loading the driver and the overlay, we should see both LEDs being turned on.

## Testing

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_dev_driver.ko

# Insert the dt overlay
sudo dtoverlay my_overlay.dtbo

# You should now see that both LEDs are on

# Remove the dt overlay
sudo dtoverlay -R my_overlay

# Now the LEDs should both be turned off again

# Unload the driver
sudo rmmod my_dev_driver
~~~
