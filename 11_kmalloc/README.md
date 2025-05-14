# 11_kmalloc

In this lesson we will talk about two topics: Dynamical memory allocation in a Kernel Module and about the `private_data` pointer in the `struct file` representing an opened file in the Linux kernel.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

## Dynamical memory allocation

In userspace we are using `malloc` for allocating memory dynamically and `free` to free the memory again. In the kernel similar functions are available. But to indicate they are kernel functions, they have a `k` prefix. So, the functions are called `kmalloc` and `kfree` and they work similar to the userspace counterparts.

`kmalloc` allocates some bytes of memory but doesn't initialize them. If you want to have your allocated memory initialized with zeros, use `kzalloc`.

`kmalloc` has two arguments. The first one is the amount of memory in bytes you want to allocate and the second one is a flag telling the kernel  how to allocate the memory. The following table shows some examples:

| flag       | description                                             |
|------------|---------------------------------------------------------|
| GFP_KERNEL | Normal allocation of kernel memory                      |
| GFP_ATOMIC | Used for allocating memory in interrupt context         |
| GFP_DMA    | Allocates physical contiguous memory for DMA operations |

Of course, if you allocate memory, don't forget to free it when you no longer need it 😉

## `private_data` in `struct file`

The `struct file` represents an opened file in the Linux kernel. It has a pointer called `private_data` to store data belonging to the file. A pointer to the `struct file` is passed to every file operation like read, write, ioctl, ... and here we can access and use the private data.

## Explaining the code

The init function allocates a range of device numbers, creates a character device and the device file for us. We will implement the read, write, open and release function in our kernel module so they can be used by the character device.

In the open function, we are allocating 64 bytes and store them in the private data pointer of the `struct file`. In the write callback we can write into these memory and in the read function we can read from it. In the release function we are freeing the memory.

To be able to read back the written memory, we have to set the file offset stored in the `loff_t off` parameter passed to read and write back to 0. Therefore, we set the llseek callback to `default_llseek`.

## Testing the code

After loading the module, the device file `/dev/hello0` appears. Let's read and write to this file:

~~~
pi@raspberry:~/Programming/Linux_Driver_Tutorial/11_kmalloc
$ echo "Hello World" | sudo tee //dev/hello0
Hello World
pi@raspberry:~/Programming/Linux_Driver_Tutorial/11_kmalloc
$ sudo cat /dev/hello0
ELF���������@8@pi@raspberry:~/Programming/Linux_Driver_Tutorial/11_kmalloc
~~~

Wait, why couldn't we read back "Hello World" and what is that garbage cat is showing. Well, the reason for that is, that the dynamically allocated memory only lives as long as the file is open and after both programs `tee` and `cat`, the device file is opened, the read or write operation is done and the file is closed again. On closing the memory is freed, so we can't read it back. 

The reason `cat` returns garbage is because we don't initialize the memory. If we would have used `kzalloc` instead, we would read back zeros and nothing would appear in the terminal.

So, let's implement a userspace program where we open the device file, write to it, set the file offset back to 0 with lseek and then read the file again. This is done in `test.c`. If we compile and run it, we can read back the written value. But after closing the file, the dynamically allocated memory gets freed in the driver and is no longer available.
