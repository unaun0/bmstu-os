#!/bin/bash

set -e
MODULE_NAME="my_module"
KO_FILE="${MODULE_NAME}.ko"
PROC_DIR="/proc/my_dir"
PROC_FILE="$PROC_DIR/my_file"
PROC_LINK="/proc/my_symlink"
echo "==> Компиляция модуля"
make
echo "==> Загрузка модуля"
if lsmod | grep "^${MODULE_NAME}"; then
    sudo rmmod $KO_FILE
fi
sudo insmod $KO_FILE
echo "==> Информация о модуле (lsmod)"
lsmod | grep "^${MODULE_NAME}"
echo "==> Информация о модуле (modinfo)"
modinfo $KO_FILE
echo "==> Проверка /proc"
ls -l $PROC_DIR
ls -l $PROC_LINK
echo "==> Запись PID 1"
echo 1 > $PROC_FILE
echo "==> Запись PID 2"
echo 2 > $PROC_FILE
echo "==> Запись PID 3"
echo 3 > $PROC_FILE
echo "==> Чтение из /proc файла"
cat $PROC_FILE
echo "==> Выгрузка модуля"
sudo rmmod $KO_FILE
echo "==> Вывод последних сообщений ядра (dmesg)"
sudo dmesg | grep "\*)" | tail -n 21
echo "==> Проверка удаления /proc записей"
if [ -d "$PROC_DIR" ]; then
    echo "Ошибка: $PROC_DIR существует"
else
    echo "$PROC_DIR успешно удалён"
fi
if [ -e "$PROC_LINK" ]; then
    echo "Ошибка: $PROC_LINK существует"
else
    echo "$PROC_LINK успешно удалён"
fi
echo "==> Очистка сборочных файлов"
make clean
echo "==> Конец"
