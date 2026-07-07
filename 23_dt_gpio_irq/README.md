# 23_dt_gpio_irq

In this lecture you will learn how to use a GPIO interrupt within the driver of a Device Tree Device.

This will only work on Linux Systems with Device Tree Support.

## Hardware setup

![LED and Button connected to Raspberry Pi](../03_gpioctrl/led_button_Steckplatine.png)

This example will not use the LED. Just the button must be connected to GPIO 20.

## Adding the interrupt to the device tree overlay

In the device tree overlay, we need to add a property to the device `my_device`:

~~~
interrupts-extended = <&gpio 20 2>;
~~~

The first argument is a pointer to the interrupt controller. Here it is the device `gpio` which is the GPIO controller of the Raspberry Pi. The next argument is the number of the GPIO pin which should trigger the interrupt. So, in our case, it is GPIO 20. The last argument are flags for the interrupt. 2 stands for Rising Edge, so the IRQ should only be generated on a rising edge of GPIO 20.

If you are using another Single Board Computer then the Raspberry Pi, the GPIO controller node can be named differently and also the number of arguments and their meaning required can differ.

## Implementing the driver

In the driver we must include `linux/interrupt.h` for some macros and functions related to interrupts.

Then we can implement the *Interrupt Service Routine* (ISR). This function will be called when the interrupt is triggered. The return value must be from the type `irqreturn_t` and if the ISR finished successfully, the function returns `IRQ_HANDLED`.

The ISR takes two arumgent. The first one is the number of the interrupt being triggered. It is possible to use the same ISR for different interrupts. The second argument is a data pointer which is passed to the ISR and can be set when requesting the interrupt.

In the probe function we read out the interrupt number from the device tree device with the function `platform_get_irq`. The first argument is the platform device, the second one the interrrupt index. A device can have more then just one interrupt. On success, the function returns the interrupt number, on failure a negative error code.

Then we are requesting the interrupt with the function `devm_request_irq`. This is a managed function, therfore the lifetime of the interrupt is bound to the lifetime of the device. If the device is removed from the system or the driver is unloaded, the kernel will free the interrupt.

The arguments of `devm_request_irq` are:

- A pointer to the device for which we request the interrupt
- The number of the interrupt
- A pointer to the ISR function
- Optional Flags
- A name which will appear on `/proc/interrupts`
- A pointer which is passed to the ISR data pointer

The function returns 0 on success, a negative error code else.

## Testing

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_dev_driver.ko

# Insert the dt overlay
sudo dtoverlay my_overlay.dtbo

# Press the button, check the kernel's log
# You will see the ISR being called!

# Check /proc/interrupts for my_dev_irq
grep my_dev_irq /proc/interrupts 

# Remove the dt overlay
sudo dtoverlay -R my_overlay

# Unload the driver
sudo rmmod my_dev_driver
~~~
