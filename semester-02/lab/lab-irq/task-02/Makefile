# Имя модуля без расширения
MODULE_NAME = wq

# Путь к заголовочным файлам ядра
KDIR := /lib/modules/$(shell uname -r)/build

# Текущая директория
PWD := $(shell pwd)

obj-m := $(MODULE_NAME).o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

load: all
	sudo insmod $(MODULE_NAME).ko

unload:
	sudo rmmod $(MODULE_NAME)

reload: unload load

log:
	sudo dmesg | grep keylogger_workq_case
