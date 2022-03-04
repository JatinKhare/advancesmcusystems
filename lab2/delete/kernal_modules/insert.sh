#!/bin/sh

make
rm /dev/cdma_int
mknod /dev/cdma_int c 241 0

rm /dev/captimer_int
mknod /dev/captimer_int c 240 0

rmmod cdma_int.ko
insmod cdma_int.ko

rmmod captimer_int.ko
insmod captimer_int.ko

echo $'\nlsmod output:\n'
lsmod

echo $'\n/proc/interrupts\n'
more /proc/interrupts > proc_interrupts.txt
tail -12 proc_interrupts.txt | head -2

