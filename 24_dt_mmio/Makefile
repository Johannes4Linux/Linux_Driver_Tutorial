obj-m += my_timer.o

all: driver rpi4_timer.dtbo rpi1_2_zero_timer.dtbo rpi3_zero2_timer.dtbo

driver:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

rpi4_timer.dtbo: rpi4_timer.dts
	dtc -I dts -O dtb $< -o $@

rpi1_2_zero_timer.dtbo: rpi1_2_zero_timer.dts
	dtc -I dts -O dtb $< -o $@

rpi3_zero2_timer.dtbo: rpi3_zero2_timer.dts
	dtc -I dts -O dtb $< -o $@

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) rpi4_timer.dtbo rpi1_2_zero_timer.dtbo rpi3_zero2_timer.dtbo
