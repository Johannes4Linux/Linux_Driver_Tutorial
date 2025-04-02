# 04_devicefiles (FreeBSD version)

## Device Files

Device files on FreeBSD are used as an interface between userland and a kernel module. Like on other \*nix systems, they can be found in the `/dev` directory. Device files are virtual files created by the _device file system (devfs)_ that represent physical or virtual devices on the system.

If we run the command `ls -l` in the `/dev` directory, we can see the device files indicated by the letter **c** in front of the permissions:

```shell
$ ls -la /dev/gpio* /dev/cuau0
crw-rw----  1 uucp dialer 0x31 Mar 31 15:16 /dev/cuau0
crw-------  1 root wheel  0x2d Mar 31 15:03 /dev/gpioc0
crw-------  1 root wheel  0x34 Mar 31 15:03 /dev/gpioc1
```

The letter **c** at the beginning of the of the permissions indicated that the devices are _character_ devices. The number 1 is the link count for the file. The next two columns are the owner and the group that owns the device. In the case of `/dev/cuau0` the owner is "uucp" and the group is "dialer". The hex number is the Device ID. Next we have the modification dates and the file name.

## Character Devices

FreeBSD only uses character devices, having removed block devices by FreeBSD 5.0. The decision to do so was because the developers found block devices redundant as buffering and caching logic could be handled by the kernel. Leaving block devices in the system would have caused relability problems, as explained by the FreeBSD handbook:

```
Other UNIX® systems may support a second type of disk device known as block devices. Block devices are disk devices for which the kernel provides caching. This caching makes block-devices almost unusable, or at least dangerously unreliable. The caching will reorder the sequence of write operations, depriving the application of the ability to know the exact disk contents at any one instant in time.
```

[^1]

## Device Numbers

Device numbers are less important on FreeBSD than they are in Linux. On FreeBSD, the devfs system manages the devices nodes automatically in kernel space. The nodes are automatically created when the devfs service starts, and major and minor numbers are assigned to device. Unlike Linux, which breaks the Device ID into major and minor numbers, FreeBSD combines the two into a hex value.

If we want to separate the major and minor number we can use the `stat -f` with command to format the number.

```shell
$ stat -f "Major: %Hr, Minor: %Lr" /dev/cuau0
Major: 0, Minor: 49
```

The "%Hr" and "%Lr" in the escape sequence tell stat to format the response, with the higher part of the hex code being formatted into the string after the word "Major: " and the lower part of the hex formatted after "Minor: ".

FreeBSD does not have the file /proc/device files. Instead if we want to get the major and minor number of a device we need to use a small shell script utilizing the stat command:

```shell
$ find /dev -type c -exec stat -f "%N Major: %Hr Minor: %Lr" {} \;
/dev/reroot/reroot Major: 0 Minor: 3
/dev/random Major: 0 Minor: 4
/dev/sndstat Major: 0 Minor: 6
/dev/devctl2 Major: 0 Minor: 7
/dev/console Major: 0 Minor: 8
/dev/geom.ctl Major: 0 Minor: 9
/dev/devctl Major: 0 Minor: 10
/dev/uinput Major: 0 Minor: 11
/dev/input/event0 Major: 0 Minor: 12
/dev/input/event1 Major: 0 Minor: 29
/dev/kbdmux0 Major: 0 Minor: 13
/dev/mem Major: 0 Minor: 15
/dev/kmem Major: 0 Minor: 16
/dev/netmap Major: 0 Minor: 17
/dev/full Major: 0 Minor: 18
/dev/null Major: 0 Minor: 19
/dev/zero Major: 0 Minor: 20
...
```

## Creating Device files

### mknod

FreeBSD has the [mknod](https://man.freebsd.org/cgi/man.cgi?query=mknod&sektion=8) command, but it is depreciated and not recommended.

If we get the device number for /dev/mmcsd0s1, 0x59, and convert it form hex to decimal we get the major number 0 and minor number 89. We can then use the `mknod` command to create a node with the arguments _c_ for character device, and _0_ and _89_ for the major and minor numbers respectively. The file will be created in the location specified (in this case the current working directory was home)

```shell
jholloway@BSD-PI4:~ $ ls -l /dev/mmcsd0s1
crw-r-----  1 root operator 0x59 Mar 31 15:03 /dev/mmcsd0s1

jholloway@BSD-PI4:~ $ sudo mknod mymmc c 0 89

jholloway@BSD-PI4:~ $ ls
01_hello                03_gpioctrl             gpioctrl-cross.ko       mymmc
jholloway@BSD-PI4:~ $ ls -l mymmc
crw-r--r--  1 root jholloway 0x59 Mar 31 15:39 mymmc
```

Using the _ls_ command we can see that the newly create node ~/mymmc has the same device number as the file /dev/mmcsd0s1. However, if we try to read from the file we will get an error:

```shell
jholloway@BSD-PI4:~ $ hexdump mymmc | head
hexdump: mymmc: Operation not supported
```

Because mknod has been depreciated, as of FreeBSD 6.0 mknod can create nodes, but the nodes cannot be used to access devices. Instead we must use the devfs utility to create nodes.

### devfs

The [devfs](https://man.freebsd.org/cgi/man.cgi?query=devfs&apropos=0&sektion=8) utility in FreeBSD manages device file system mounts, which provide access to kernel-managed devices in userland through the /dev directory. The device file system (devfs) is a virtual filesystem that dynamically creates and manages device nodes corresponding to hardware and pseudo-devices recognized by the kernel

We can copy an existing node using the devfs utility. First we can edit `/etc/devfs.conf` and add the following line to create a new node called mymmc that links to mmcsd0s1:

```make
$ ls -l /dev/mmcsd0s1 /dev/mymmc
crw-r-----  1 root operator 0x59 Mar 31 15:03 /dev/mmcsd0s1
lrwxr-xr-x  1 root wheel       8 Mar 31 16:06 /dev/mymmc -> mmcsd0s1
```

Next we need to restart devfs service to force the system to recreate the device nodes:

```shell
# service devfs restart
```

In addition to _mmcsd0s1_ inside of /dev, we will also see a new node in the directory called _mymmc_

```shell
$ ls -l /dev/mmcsd0s1 /dev/mymmc
crw-r-----  1 root operator 0x59 Mar 31 15:03 /dev/mmcsd0s1
lrwxr-xr-x  1 root wheel       8 Mar 31 16:06 /dev/mymmc -> mmcsd0s1
```

If we compare the original node /dev/mmcsd0s1 with that of /dev/mymmc we will see that it has a different device number, but it will specify that mymmc is a link to mmcsd0s1. This differs from the node created with the `mknode` command, which had the same device id as the original node. Unlike the node we made with the previous method, we can read and write to this device as illustrated with using hexdump:

```shell
$ sudo hexdump /dev/mmcsd0s1 | head
0000000 3ceb 4290 4453 2e34 2034 0020 0802 0001
0000010 0002 0002 f000 0032 003f 00ff 0000 0000
0000020 9000 0001 0000 0729 cb0f 4526 4946 2020
0000030 2020 2020 2020 4146 3154 2036 2020 31fa
0000040 8ec0 bcd0 7c00 8efb e8d8 0000 835e 19c6
0000050 07bb fc00 84ac 74c0 b406 cd0e eb10 30f5
0000060 cde4 cd16 0d19 4e0a 6e6f 732d 7379 6574
0000070 206d 6964 6b73 0a0d 7250 7365 2073 6e61
0000080 2079 656b 2079 6f74 7220 6265 6f6f 0d74
0000090 000a 0000 0000 0000 0000 0000 0000 0000

$ sudo hexdump /dev/mymmc  | head
0000000 3ceb 4290 4453 2e34 2034 0020 0802 0001
0000010 0002 0002 f000 0032 003f 00ff 0000 0000
0000020 9000 0001 0000 0729 cb0f 4526 4946 2020
0000030 2020 2020 2020 4146 3154 2036 2020 31fa
0000040 8ec0 bcd0 7c00 8efb e8d8 0000 835e 19c6
0000050 07bb fc00 84ac 74c0 b406 cd0e eb10 30f5
0000060 cde4 cd16 0d19 4e0a 6e6f 732d 7379 6574
0000070 206d 6964 6b73 0a0d 7250 7365 2073 6e61
0000080 2079 656b 2079 6f74 7220 6265 6f6f 0d74
0000090 000a 0000 0000 0000 0000 0000 0000 0000
```

[^1] : https://www.freebsd.org/doc/en/books/arch-handbook/driverbasics-block.html
