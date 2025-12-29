# 17_dt_read_property

In this lecture you will learn how to parse properties from the device tree device.

## Device Tree Properties

Our device tree device has the following properties:

- compatible: Needed for matching with the correct driver
- status: En- or disables the device
- string_var: Custome string property
- int_var: Custome integer variable

The last two properties are nonstandard properties as stated in the [Device Tree Specification](https://www.devicetree.org/specifications). If you want to be compliant with this specification, you need to add a vendor prefix before the property (e.g. brightlight,string_var). In general it is recommended to try to avoid nonstandard properties. In the DT Specification you can find all standard properties.

## Implementing the driver

In the probe function we will parse the last two properties. The functions needed for doing so are defined in `linux/property.h`. Before reading in a property, the function `device_property_present` can be used to check if a property is present. The function needs a pointer to a device and the property name and retruns 0 if the property is not present or 1 if it is.

To read in a property the function `device_property_read_<datatype>` can be used. `<datatype>` must be replaced with the desired datatype. For reading in the int_var, the function `device_property_32` is used, for string_var, `device_property_string`. The arguments are the same as for `device_property_present` only a pointer at which the property will be stored must be added. The function returns 0 on success and a negative error code else.

## Testing

~~~
# Compile the code and the dt overlay
make
# Load driver
sudo insmod my_dev_driver.ko

# Insert the dt overlay
sudo dtoverlay my_overlay.dtbo
# If you check the kernel's log know, you should see the device properties:
[  574.316863] my_dev - int_var: 123
[  574.316880] my_dev - string_var: Hello, DT!
~~~
