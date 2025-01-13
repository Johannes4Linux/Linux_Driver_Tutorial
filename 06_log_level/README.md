# 06_kernel_logging

Here you can see how different log levels can be used with printk and how they are formated at the kernel log.

This example can be compiled and run on a Raspberry Pi or a normal x86 computer.

You can find the information also in the [Linux Kernel documentation](https://www.kernel.org/doc/html/latest/core-api/printk-basics.html)

## printk Function

Printk is used like this:

~~~
printk(KERN_INFO "Message: %s\n", arg);
~~~

KERN_INFO is the log level used for this line of the kernel logs

## Kernel Log Levels

The following log levels are available:

|     Name     | String |                 Alias function                |
|:------------:|:------:|:---------------------------------------------:|
| KERN_EMERG   | “0”    | pr_emerg()                                    |
| KERN_ALERT   | “1”    | pr_alert()                                    |
| KERN_CRIT    | “2”    | pr_crit()                                     |
| KERN_ERR     | “3”    | pr_err()                                      |
| KERN_WARNING | “4”    | pr_warn()                                     |
| KERN_NOTICE  | “5”    | pr_notice()                                   |
| KERN_INFO    | “6”    | pr_info()                                     |
| KERN_DEBUG   | “7”    | pr_debug() and pr_devel() if DEBUG is defined |
| KERN_DEFAULT | “”     |                                               |
| KERN_CONT    | “c”    | pr_cont()                                     |

## Filtering for log levels

You can filter for a specific loglevel with the -l option of dmesg:

~~~
# Only show Debug messages
sudo dmesg -l 7

# Only show critical messages
sudo dmesg -l 2
~~~
