#!/bin/bash

sudo losetup /dev/loop17 floppy.img
sudo mount /dev/loop17 /mnt/p1
sudo cp kernel /mnt/p1/kernel
sudo umount /mnt/p1
sudo losetup -d /dev/loop17
