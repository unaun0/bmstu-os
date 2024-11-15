#!/bin/bash

# Установка gdown, если она не установлена
if ! command -v gdown &> /dev/null; then
  echo "Установка gdown..."
  pip install gdown
fi

# Установка wget, если она не установлена
if ! command -v wget &> /dev/null; then
  echo "Установка wget..."
  brew install wget
fi

# Скачивание файла ISO
FILE_ID="1-wh6GlOFhkP4b0aLdpE8TDtne-6vdD6I"
if [ -n "$FILE_ID" ]; then
  echo "Скачивание файла ISO..."
  gdown https://drive.google.com/uc?id=$FILE_ID
else
  echo "Ошибка: не указан ID файла"
  exit 1
fi

# Получение direct ссылки на файл из VK
FILE_LINK="https://vk.com/doc781359414_677916544"
if [ -n "$FILE_LINK" ]; then
  echo "Получение direct ссылки на файл..."
  DIRECT_LINK=$(wget -q -O - "$FILE_LINK" | grep -o 'https://[a-zA-Z0-9./?=]*' | grep -o 'https://[a-zA-Z0-9./?=]*\.iso')
  if [ -n "$DIRECT_LINK" ]; then
    echo "Скачивание файла ISO..."
    wget -q -O - "$DIRECT_LINK"
  else
    echo "Ошибка: не удалось получить direct ссылку на файл"
    exit 1
  fi
else
  echo "Ошибка: не указана ссылка на файл"
  exit 1
fi

echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "!INFO: Установи Parallels согласно инструкциям! :)    !"
echo "!INFO: Ключ Windows XP - DPDQK-H94J9-TWMTR-K2K69-FRKHM!"
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"