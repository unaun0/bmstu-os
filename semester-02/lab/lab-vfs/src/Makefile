CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET = main
obj-m := $(TARGET).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order Module.*
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions
	@rm *.ko *.mod
	@rm *.symvers

load:
	sudo insmod ${TARGET}.ko

remove:
	sudo rmmod ${TARGET}.ko

minfo:
	modinfo ${TARGET}.ko
	sudo lsmod | grep main

logs:
	sudo dmesg | grep myvfs

mount:
	@mkdir fs_dir
	sudo mount -t myvfs none ./fs_dir

fsinfo:
	cat /proc/filesystems | grep myvfs
	mount | grep myvfs

unmount:
	sudo umount ./fs_dir
	@rm -rf fs_dir
	
sinfo:
	sudo cat /proc/slabinfo | grep myvfs

	
