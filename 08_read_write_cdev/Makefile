obj-m += hello_cdev.o

all: testapp
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm testapp

testapp: test.c
	gcc $< -o $@
