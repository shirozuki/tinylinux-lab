CC      ?= gcc
CFLAGS  += -static -Wall -Wextra -O2

LINUX   ?= ../src/linux
KERNEL  ?= $(LINUX)/arch/x86/boot/bzImage
BUSYBOX ?= ../src/busybox/_install

all: initramfs.cpio.gz

init: init.c
	$(CC) $(CFLAGS) -o $@ init.c

initramfs.cpio.gz: init
	rm -rf rootfs
	mkdir -p rootfs/proc rootfs/sys rootfs/dev
	cp -a $(BUSYBOX)/. rootfs/
	cp init rootfs/init
	(cd rootfs && find . -print0 | cpio --null -o -H newc | gzip) > $@

run: initramfs.cpio.gz
	qemu-system-x86_64 -enable-kvm -m 1G -nographic -no-reboot \
	  -kernel $(KERNEL) -initrd initramfs.cpio.gz \
	  -append "console=ttyS0 panic=-1"

kernel:
	$(MAKE) -C $(LINUX) -j$$(nproc)

clean:
	rm -rf init rootfs initramfs.cpio.gz

.PHONY: all run kernel clean
