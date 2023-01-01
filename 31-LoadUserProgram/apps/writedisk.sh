#!/bin/bash

echo "write disk start.."
part=$(sudo kpartx -av ../../mkfile/hda.img)
echo $part
devmap=$(echo $part | awk '{print $3}')
sudo mount /dev/mapper/$devmap /mnt/p1
sudo cp ./shell ./pwd /mnt/p1/bin/
sudo umount /mnt/p1
sudo kpartx -d ../../mkfile/hda.img
