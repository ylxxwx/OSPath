#!/bin/bash
# run_bochs.sh

qemu-system-x86_64 -drive file=floppy.img,index=0,if=floppy
