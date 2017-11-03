obj-m := module_dev_creat_kernel.o 
all:
	make -C /lib/modules/4.10.0-37-generic/build M=$(shell pwd) modules 
clean:
	make -C /lib/modules/4.10.0-37-generic/buile M=$(shell pwd) modules clean
