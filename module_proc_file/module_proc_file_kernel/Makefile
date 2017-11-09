obj-m := module_proc_file.o
all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules 
clean:
		make -C /lib/modules/$(shell uname -r)/buile M=$(shell pwd) modules clean
