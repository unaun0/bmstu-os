# Указываем объектный файл для модуля
obj-m += fortune.o

# Получаем путь к исходникам ядра Linux из переменной окружения KDIR или используем значение по умолчанию
KDIR ?= /lib/modules/$(shell uname -r)/build

# Флаги
ccflags-y += -std=gnu18 -Wall -Werror

build:
	make -C $(KDIR) M=$(shell pwd) modules

# Правило для сборки модуля
all: build

# Правило для очистки директории от лишних файлов
clean:
	make -C $(KDIR) M=$(shell pwd) clean

install: build
	sudo make -C $(KDIR) M=$(shell pwd) modules_install