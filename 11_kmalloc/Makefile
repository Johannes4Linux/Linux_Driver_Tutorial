obj-m += hello_cdev.o

all: hello_cdev.ko t

hello_cdev.ko: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

t: test.c
	gcc $< -o $@

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) t
