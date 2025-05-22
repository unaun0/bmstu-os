#!/bin/bash
# set -Eeuxo pipefail

make all
sudo insmod fortune.ko
lsmod | grep fortune

sudo dmesg | grep fortune

echo "12345" > /proc/fortuneFile
echo "gfhg" > /proc/fortuneFile
echo "chto-to" > /proc/fortuneFile

cat /proc/fortuneFile
cat /proc/fortuneFile
cat /proc/fortuneFile

ls -ail /proc | grep fortune

sudo rmmod fortune

sudo dmesg | grep fortune

make clean
