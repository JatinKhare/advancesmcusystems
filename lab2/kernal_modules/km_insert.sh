#!/bin/sh


make clean
make

rm /dev/cdma_int
mknod /dev/cdma_int c 241 0

#rmmod cdma_int.ko
insmod cdma_int.ko

rm /dev/captimer_int
mknod /dev/captimer_int c 240 0

#rmmod captimer_int.ko
insmod captimer_int.ko

