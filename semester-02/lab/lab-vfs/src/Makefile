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

disclean: clean remove_files
	@rm *.ko *.symvers *.mod

load:
	sudo insmod ${TARGET}.ko

remove:
	sudo rmmod ${TARGET}.ko

modelue_info:
	modinfo ${TARGET}.ko

logs:
	dmesg | grep a23fs

create_files:
	@touch daemontool
	@mkdir fs_dir

remove_files:
	@rm daemontool
	@rm -rf fs_dir

mount:
	sudo mount -o loop -t a23fs ./daemontool ./fs_dir

fs_info:
	cat /proc/filesystems | grep a23fs
	mount | grep a23fs

unmount:
	sudo umount ./fs_dir
	
slab_info:
	sudo cat /proc/slabinfo | grep a23fs

	
