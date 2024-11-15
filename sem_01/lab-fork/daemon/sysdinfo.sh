#!/bin/bash

# Проверяем, есть ли права на выполнение
if [[ $EUID -ne 0 ]]; then
   echo "Скрипт должен быть запущен от имени суперпользователя." >&2
   exit 1
fi

# Имя файла для вывода
output_file="sys_daemons_info.txt"

# Открываем файл для записи
{
    # Получаем список всех демонов
    for service in $(launchctl list | awk '{print $3}' | tail -n +2); do
        # Формируем полный путь
        full_service_id="system/$service"
        
        # Проверяем существование
        if launchctl list | grep -q "$service"; then
            # Проверяем, является ли это демоном LaunchDaemon
            if [[ "$service" == com.* ]]; then
                echo "Детали для $full_service_id:"
                launchctl print "$full_service_id"
                echo "--------------------------------------------------------------------------------"
            fi
        else
            echo "$full_service_id не существует."
            echo "--------------------------------------------------------------------------------"
        fi
    done
} > "$output_file"

echo "Информация о демонах записана в файл $output_file."

exit 0

# launchctl list
# ps -ajx
# ps -axj | grep ./mydaemon
# root             21864     1 21864      0    0 Ss     ??    0:00.00 ./mydaemon

