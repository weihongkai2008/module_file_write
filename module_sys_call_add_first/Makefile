obj-m := module_sys_call_add_first.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules clean
