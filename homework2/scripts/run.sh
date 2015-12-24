qemu-system-x86_64 -M pc -kernel arch/x86/boot/bzImage -hda rootfs-2.6.30.6.ext4 -netdev user,id=network0 -device e1000,netdev=network0 -nographic -append "root=/dev/sda console=ttyS0"
