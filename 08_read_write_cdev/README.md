# 08_read_write_cdev

A demonstration for how to read from and write to a Character Device Files in a Linux Driver or Kernel Module

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Explaining the code

This kernel module will offer a 64-Byte text Array which can be writen to and read from the device file from userspace.

The test Array is declared as a `static` global variable. The reason why `static`was choosen is that the variable can only be accessed from within the module.

Let's take a look at the arguments of the read function:

- `struct file *filp`: Pointer to a `struct file`which represents an opened file in the Linux Kernel.
- `char __user *user_buf`: Pointer to where the read data should be saved after the read. `__user` is just a macro to increase readability and highlight that this pointerpoints to an userspace buffer
- `size_t len`: Length of the userspace buffer in bytes
- `loff_t *off`: Pointer to the current file offset. In our example, this is the current reading or writing position within the `text` array.

The read function returns the amount of successfully read bytes.

First, we are calculating the amount of bytes we can actually read. Of course, we can only read out 64 bytes as this is the size of `text`. The amount of bytes to read depends on the current file positon and the length of the userspace buffer.

Then we are using the `copy_to_user` function to copy the data from our current positon in `text`to `user_buf`. The function returns the amount of bytes which could not be copied.

Then we increse the file position by the amount of successfully read bytes and return this value.

The write callback function is quite similar, the only differences are:

- `const char __user *user_buf`: The data from userspace are `const` as they will be copied into the text buffer
- `copy_from_user` is used to transfer the data from userspace to kernelspace

The last thing to do is to add our callback functions in the `fops` struct.

## Testing the code

After loading the kernel module, you should get a print in the kernel's log:

~~~
[ 3038.299101] hello_cdev - Major Device Number: 236
~~~


Now, let's create a Device File with the corresponding Device Number:

~~~
sudo mknod /dev/hello0 c 236 0
~~~

Now, let's test the driver by writing and reading from the device file:

~~~
echo "Hello World!" | sudo tee /dev/hello0
~~~

We are using `tee` in order to write to `/dev/hello` as root user. If we would run

~~~
echo "Hello World!" > /dev/hello0
~~~

only the `echo`would be executed as root, but the write would be done with regular user permissions and therefore fail (depending on the file permissions of `/dev/hello0`)

After writing to the file, we see that the write callback function was executed once. To read back the data, run

~~~
sudo cat /dev/hello0
~~~

Here we can see, that the read callback was executed twice. The reason for this is that `cat` only terminates, when it reaches the end of file `EOF`. This is reached, when read returns 0. So, on the first read, `off` was 0, we read out 64 bytes, in the second read `off` was set to 64 which is the size of `text` and the read function returned 0.

In the test.c application we only read out one byte at a time. If we run the `testapp` with

~~~
sudo ./testapp
~~~

we can see in the kernel's log that the read function is called 65 times and the `off` is increasing on every read by one.

