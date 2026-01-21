obj-m += my_dev_driver.o

all: driver my_overlay.dtbo

driver:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

my_overlay.dtbo: my_overlay.dts
	dtc -I dts -O dtb $< -o $@

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) my_overlay.dtbo
