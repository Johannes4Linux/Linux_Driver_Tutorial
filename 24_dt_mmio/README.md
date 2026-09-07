# 24_dt_mmio

In this lecture you will learn how to access memory mapped IO from a Linux driver. This is required when writing a driver for a peripheral integrated in your System on a Chip (SoC) or when writing a driver for a piece of hardware in a FPGA connected to a processor.

The Peripheral we are using for this tutorial is only available on the chips of the Raspberry Pi 1, 2, Zero, Zero 2, 3 and 4. The Raspberry Pi 5 will not work.

## Timer (ARM Side)

For this lecture I needed a peripheral inside the chip of the Raspberry Pis for which no driver is loaded by default. I read over the [ARM Peripheral Specifications](https://pip-assets.raspberrypi.com/categories/545-raspberry-pi-4-model-b/documents/RP-008248-DS-1-bcm2711-peripherals.pdf) of the Broadcom chips used for RPi 1, 2, 3 and 4 and found out that they all have the same periperhals integrated and that there is a timer inside the chip for which no driver is loaded.

This is a simple 16- or 32-bit free running timer. The processor can access the registers of the timer by accessing specific physical addresses. The addresses depends on the chip used on the Raspberry Pi. The following table shows the address of the timer for different RPi versions:

| Modell                  | Timer physical address |
|-------------------------|------------------------|
| Raspberry Pi 1 / Zero   | 0x2000_b400            |
| Raspberry Pi 2          | 0x2000_b400            |
| Raspberry Pi 3 / Zero 2 | 0x3f00_b400            |
| Raspberry Pi 4          | 0xfe00_b400            |

The addesses can be caluclated from the ARM Peripherals documents and the different address views.

Today, we will only access the first four registers of the timer: The timer can be configured and en- or disabled over the *CONTROL* register. *VALUE* contains the current counter value and *LOAD* is the value which will be loaded into *VALUE* when the timer expires. So, let's write a driver to start the timer and accessing the registers.

## Device tree overlay

First, we have to modify the device tree overlay. The name of the device is *my_timer* The physical address of the timer is added to the name after an `@` sign. The property `reg` provides the address over which the timer registers can be accessed. The `reg` property is split into an address value and a size value. The address value provides the phyiscal address and the size value the amount of bytes which can be accessed from this address.

The parameter `#address-cells` specifies how many 32-bit words are used for the address value, `#size-cells` how many 32-bit words are used for the size value. Here, we use 2 * 32-bit = 64-bit for the address and 1 * 32-bit for the size.

## Writing the Linux Device Driver

In the driver `linux/io.h` is included for the functions to get and map the resource. We declare two global variables. `struct resource res` represents the `reg` property from the device tree and `struct u32 __iomem *hwregs` is the pointer over which we will be able to access the timer's registers.

With `platform_get_resource` we read out `reg` entry `0` from the platform device `pdev` with the type `IORESOURE_MEM`. On success we get a valid pointer to a `struct resource` else a `NULL` pointer.

The field `res->start` contains the start address of the resource. With `resource_size(res)` we get the size of the resource.

Now, we have read in the `reg` property of the device tree entry. The next step is to retrieve a pointer to these registers. Therefore, `devm_ioremap` is used. As it is a managed function, the livetime of the pointer is bound to the device. The first argument is the device for which the memory is mapped. The second one is the physical start address and the last one the size. On success, we get a valid pointer. By accessing the pointer, we are accessing the hardware registers of the timer.

## Testing

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_time.ko

# Insert the dt overlay depending on your RPi version
sudo dtoverlay rpi4_timer.dtbo
# If you check the kernel's log know, you should see the print
[ 7035.500946] my_timer: loading out-of-tree module taints kernel.
[ 7035.504230] my_timer_driver fe00b400.my_timer: Resource at fe00b400 (64 Bytes in size)
[ 7035.504273] my_timer_driver fe00b400.my_timer: Timer Value: 0x3268
[ 7035.504285] my_timer_driver fe00b400.my_timer: Timer Value: 0xfdac

# Remove the dt overlay
sudo dtoverlay -R rpi4_timer

# Unload the driver
sudo rmmod my_timer
~~~
