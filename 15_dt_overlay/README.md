# 15_dt_overlay

This example will only work on a Raspberry Pi with Raspberry Pi OS!

This example shows how to add a first device tree overlay. The device tree has a human readable and also a compiled machine readable representation. The following table sumarizes the various forms of the device tree.

| File ending | Abbriviation               | Readable for |
| ----------- | -------------------------- | ------------ |
| dts         | Device Tree Source         | Human        | 
| dtsi        | Device Tree Source Include | Human        | 
| dtb         | Device Tree Binary         | Machine      |
| dtbo        | Device Tree Binary Overlay | Machine      |

## Explaining the dts

For our device tree overlay, we must create a device tree source file `my_overlay.dts`.

At the start of the file we provide some information about the device tree version we are using `/dts-v1/;` and the `/plugin/;` tells the reader that this device tree source is for an overlay and not for a regular device tree.

Now we can add fragments to the device tree. Each fragment has its own target o target path which tells where to insert the overlay. If we want to add a device to the I2C bus 1, the overlay should look like this:

~~~
/{
	fragment@0 {
		target = <&i2c1>;
		__overlay__ {
            // Add device here 
        };
    };
};
~~~

But we add a device to the root of the device tree. Therefore we set the target-path to `/ `.

A device needs a device node name which is `my_device` in this example. Before the node there can be a label (here: `my_dev_label`) and at the end of the device node an address can be added after the node and a `@ ` sign, e.g. `my_device@A0001000. The interpretation of the address depends where this device is added. If it is mapped to the physical address space of the processor, the address must be a physical address, if it is added to the I2C bus, it must be an I2C address. But the address is optional.

Then we add some properties to `my_device`. The first one is the `compatible` string. Over it the link between the device tree node and the driver which should be used for it, is done. Normally the string is split in a vendor name (brightlight) and a device name (mydev) separated with a comma.

The next property enables the device. If we set `status` to `"disable"` the device is disabled and can be enabled later via another device tree overlay.

The last two properties are custome properties. One contains a string and the second an integer variable. Intergers must be put in `<` and `>` braces.


## Compiling the Device Tree Overlay

In order to compile the Device Tree Overlay, run the following command:

~~~
dtc -I dts -O dtb my_overlay.dts -o my_overlay.dtbo
~~~

`-I ` tells the input format which is device tree source, `-O` the output format which is a device tree binary. Then we must add the filename of the input file and after the `-o` option the utput filename.

## Applying the Device Tree Overlay

Raspberry Pi OS includes a very easy tool to apply a device tree overlay. The tool is called `dtoverlay` and to apply our overlay you just have to run the following command:

~~~
sudo dtoverlay my_overlay.dtbo
~~~

On other systems with device tree support, there are other generic ways to load a device tree overlay, but I will not cover them here.

To remove your overlay, you can also use the tool:

~~~
sudo dtoverlay -R my_overlay
~~~

# Inspect the device tree

After adding the overlay, you should see a new folder in `/proc/device-tree` with the name `my_device`. In this folder each property of the device must be present as a file:

~~~
$ ls /proc/device-tree/my_device/
compatible  int_var  name  status  string_var
~~~

If you inspect the files with cat and hexdump, you can see the properties are set as in our device tree source.

