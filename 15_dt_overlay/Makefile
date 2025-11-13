all: my_overlay.dtbo

my_overlay.dtbo: my_overlay.dts
	dtc -I dts -O dtb $< -o $@

clean:
	$(RM) my_overlay.dtbo
