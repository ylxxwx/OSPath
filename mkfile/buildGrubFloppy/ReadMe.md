# 1 Build

In any task directory, `make kernel.img`, copy the kernel.img to this dir and rename it to `kernel`

# 2 Write to floppy

Run writeimg.sh to write the kernel to the floppy

# 3 Run Grub booting

use `run_bochs.sh` or `run_qemu.sh` to run this image.
