# 25_dt_irq

In this lecture you will learn how to use an interrupt connected to the interrupt controller of a System on a Chip. The ARM Timer inside the Broadcom chip of the Raspberry Pi has one interrupt. Let's find out how we can use it in a Linux driver.

The Peripheral we are using for this tutorial is only available on the chips of the Raspberry Pi 1, 2, Zero, Zero 2, 3 and 4. The Raspberry Pi 5 will not work.

## Timer (ARM Side)

For details see the [ARM Peripheral Specifications](https://pip-assets.raspberrypi.com/categories/545-raspberry-pi-4-model-b/documents/RP-008248-DS-1-bcm2711-peripherals.pdf) of the Broadcom chips used for the Raspberry Pis.

The Time offers one interrupt and some registers to use the interrupts. The register *RAWIRQ* is 1 if an interrupt is pending. A pending interrupt can be cleared by writing a 1 to the register *IRQCNTL*. In order to enable the interrupt, the Bit *IE* (Interrupt Enable, Bit 7) in the register *CONTROL* must be set.

To use the interrupt, we must find out how it is connected to the processor core(s) of the Raspberry Pi.

## Interrupt Controller

So, we need to find out which interrupt controller is used on the RPi and how we can add the interrupt of the timer. To get the interrupt, lets convert the device tree used on the RPi into a dts file:

~~~
dts -I fs -O dts /proc/device-tree -o /tmp/rpi.dts
~~~

Let's take a look at the output file `/tmp/rpi.dts` and search for `interrupt-parent`. On my system it is set to `<0x01>`. This is the name of the phandle which represents the interrupt controller. So, let's search for this phandle with `phandle = <0x01>`. In the `compatible` string we see which type of interrupt controller is used. This differs with the version of the RPi. By searching for the phyiscal address of the interrupt controller we also find out the label of it.

### RPi 1 - 3

On the older RPis the interrupt controller `brcm,bcm2835-armctrl-ic` is used. To get information about this controller, let's consult the [documentation of the Linux kernel](https://github.com/torvalds/linux/blob/master/Documentation/devicetree/bindings/interrupt-controller/brcm%2Cbcm2835-armctrl-ic.yaml). Here we see that an interrupt for this controller needs two 32-bit values. The first defines the bank, the second one the interrupt number. For our ARM timer, we must use Bank 0 and interrupt 0.

The label of the interrupt controller is `intc`. This can be found in the `aliases` section, e.g. for the RPi 1:

~~~
intc = "/soc/interrupt-controller@7e00b200";
~~~

### RPi 4

For the RPi 4 the interrupt controller `arm,gic-400` is used. To get information about this controller, let's consult the [documentation of the Linux kernel](https://github.com/torvalds/linux/blob/master/Documentation/devicetree/bindings/interrupt-controller/arm%2Cgic.yaml). Here we see an interrupt for this controller needs three 32-bit values. The first on is the type of the interrupt, the second one the interrupt number and the last one is a flag.

In the ARM Peripheral Specification we can find out that the ARM timer is using the SPI 64. SPI stands for Shared peripheral interrupt. 

The label of the interrupt controller is `gicv2`. This can be found in the `aliases` section.

## Device tree overlay

First, we have to modify the device tree overlay. We add the property `interrupt-parent` to specify which interrupt controller should be used. On the RPi 4 we set this property to <&gicv2>, on the older RPis to <&intc>.

In the device we add the property `interrupts`. For the RPi 4 we can set this to <0x00 0x20 0x4>. 0x00 stands for SPI interrupt type, 0x20 is the IRQ number and 0x4 the flag `active high level-sensitive`. For the older RPis we set it to <0x00 0x00>. The first 0x00 is for the Bank 0 and the second 0x00 is the interrupt number.

## Writing the Linux Device Driver

The header `linux/interrupt.h` is included for macros, types and functions for interrupts. Then we can implement the interrupt service routine which will be executed when the IRQ occurs. The return value is from the type `irqreturn_t`. The function takes two arguments, the first is the interrupt number, the second one a pointer to data passed to the ISR. In the function we convert the data to a `struct device *` and print out a line to the kernel's log that the IRQ occurred. We check if an IRQ is pending, but reading the *RAWIRQ* register and if so, we acknowledge it by writing a 1 to the *IRQCNTL* register.

In the probe function, we need to read the interrupt from the device. Therefore, we use the function `platform_get_irq`. The first argument is a pointer to the device, the second one the index of the IRQ. The function returns an interrupt number or a negative error code.

With `devm_request_irq` we are setting up the IRQ with a proper name and an ISR which should be triggered. The first argument is a pointer to the device, the next one the interrupt number, then we need to pass a pointer to the ISR. The next arguments are flags, we can set it to 0. Then we have to give the interrupt a name which will appear in `/proc/interrupts`. The last argument are the data which will be passed to the ISR. Here we are passing a pointer to the device.

The function `devm_request_irq` is a managed function, so the lifetime of the interrupt is bound to the lifetime of the device. As so, we don't have to free the IRQ in the remove function.

Then we configure the timer by writing a value in the *LOAD* register. Please adjust the value on the older RPis. We enable the timer in 32-bit mode with interrupts by writing to the *CONTROL* register.

## Testing

When loading the driver and the device tree overlay, we should see that the IRQ is triggered every few (milli-)seconds.

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_time.ko

# Insert the dt overlay depending on your RPi version
sudo dtoverlay rpi4_timer.dtbo
# If you check the kernel's log know, you should see the print
[ 7035.504273] my_timer_driver fe00b400.my_timer: IRQ 32 occurred!
[ 7035.504285] my_timer_driver fe00b400.my_timer: IRQ 32 occurred!

# Remove the dt overlay
sudo dtoverlay -R rpi4_timer

# Unload the driver
sudo rmmod my_timer
~~~
