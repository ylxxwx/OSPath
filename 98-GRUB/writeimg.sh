#!/bin/bash

sudo losetup /dev/loop15 floppy.img
sudo mount /dev/loop15 /mnt
sudo cp kernel /mnt/kernel
sudo umount /dev/loop15
sudo losetup -d /dev/loop15
