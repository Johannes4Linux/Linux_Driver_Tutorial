# 13_ioctl

After the introduction to `ioctl`, let's properly use it in our Linux Driver.

## Randomising ioctl numbers

Previously, I just used integer number for the `ioctl` commands. But it is desirable that the commands are more unique for each device. The idea is, that you can't use the commands from one device with the wrong driver. The commands will be defined in a header file which can be included in the source code for the kernel driver and the userspace application.

Here is how to define some commands:

- `#define RD_VAL _IOR('a', 'c', int *)`: The macro `_IOR` will create a number for the command `RD_VAL`. The `R` tells you, this command will read from kernel space. The first two values are random bytes for identifying the command. The last one is for increasing readability and telling you on one glance which argument the command takes. Here it is a pointer to an `int`.
- `#define WR_VAL _IOW('a', 'b', int *)`: The macro `_IOW` will create a number for the command `WR_VAL`. The `W` tells you, this command will write to kernel space.

## ioctl in the driver

In the `ioctl`callback function you normally add a `switch case` for the supported commands. In the `default` branch, you return the value `-EOPNOTSUPP` to indicate the command is not supported by the device. Then you add branches for the various commands.

To move data between kernel and user space, use `copy_to_user` and `copy_from_user` like shown in 08_read_write.

Here is, what the three supported commands does:

- `RD_VAL`: Read the global variable `int answer` of the kernel module and return it to user space
- `WR_VAL`: Write the global variable `int answer` of the kernel module
- `GREET`: Prints out `s.repeat` times greetings to `s.name`

## ioctl in the user space app

In the user space application, we call different commands depending on the amount of arguments passed to the program:

- No argument passed: We read out the answer with `RD_VAL`
- One argument passed: We write the answer with `RD_VAL`
- Two argument passed: We call the `GREET` command
- More then two arguments passed: An invalid `ioctl` will be inovked

## Testing the code

Just load the kernel module with `insmod` and then run the compiled userspace program with different amounts of arguments. You will see prints in the kernel's log telling you, that the ioctl function was triggered.
