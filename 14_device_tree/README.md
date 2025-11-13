# The Device Tree

## Hardware detection on x86

- Discoverable buses like PCI(e) or USB
- Standardized addresses, e.g. 0x03F8 - 0x03FF: first Serial Port, 0x0CF8 - 0x0CFC: PCI configuration space access, ... (see [osdev wiki](https://wiki.osdev.org/I/O_Ports))
- BIOS or UEFI provides information about connected hardware through Advanced Configuration and Power Interface (ACPI)

These mechanisms are normally not available on Embedded Systems -> Hardware Detection through Device Tree

## The Device Tree

- Device Tree provides the Linux Kernel information about the available devices
- Device Tree summarizes the available devices in a tree structure
- The Device Tree Sources (dts) and Device Tree Source Includes (dtsi) must be compiled (dtb: Device Tree Binary)
- Devices are present as device nodes
- Device Tree available under `/sys/firmware/devicetree/base` and `/proc/device-tree/`
- Convert to readable form: `dtc -I fs -O dts -s /sys/firmware/devicetree/base > /tmp/dt.dts`
- Device tree can also be extended via overlays ➞ not the whole device tree has to be recompiled if a device is added

## Important fields of a device tree node

- `compatible` is used to connect the device to a driver
- `status` can be used to enable ("okay") or disable ("disabled") a device
- `reg` contains the bus address and optional the size of the registers for the device
- `interrputs` property tells the interrupt number or GPIO pin

