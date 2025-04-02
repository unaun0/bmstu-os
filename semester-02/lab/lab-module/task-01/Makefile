obj-m += my_module.o
ccflags-y += -std=gnu18 -Wall

CURRENT ?= $(shell uname -r)
KDIR ?= /lib/modules/$(CURRENT)/build
PWD ?= $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

load:
 	sudo insmod $(PWD)/my_module.ko
 	dmesg | tail -n 20

unload:
 	sudo rmmod my_module
 	dmesg | tail -n 20

clean:
 	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
 	@rm -f .*.*.cmd *~ *.*~ TODO.*
 	@rm -fR .tmp*
 	@rm -rf .tmp_versions

disclean: clean
 	@rm *.ko *.symvers
