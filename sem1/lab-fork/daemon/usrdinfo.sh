#!/bin/bash

# Имя файла для вывода
output_file="usr_daemons_info.txt"

# Получаем UID текущего пользователя
user_uid=$(id -u)

# Открываем файл для записи
{
    # Получаем список всех пользовательских демонов
    for service in $(launchctl list | awk '{print $3}' | tail -n +2); do
        # Формируем полный путь к пользовательской службе
        full_service_id="user/$user_uid/$service"
        
        # Проверяем, существует ли служба
        if launchctl list | grep -q "$service"; then
            echo "Детали для $full_service_id:"
            launchctl print "$full_service_id"
            echo "--------------------------------------------------------------------------------"
        fi
    done
} > "$output_file"

exit 0
