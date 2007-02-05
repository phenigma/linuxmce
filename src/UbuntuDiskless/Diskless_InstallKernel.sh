#!/bin/bash

. /usr/pluto/bin/pluto.func

set -e

if [[ "$#" -ne 4 ]]; then
	echo "Syntax: $0 <Device>"
	exit 1
fi
Moon_DeviceID="$3"
Moon_RootLocation="/usr/pluto/diskless/${Diskless_DeviceID}"

## Determine what kernel are we going to use
Moon_KernelVersion=$(basename $(ls /boot/vmlinuz-* | head -1) | cut -d"-" -f2-99)

## Move kernel from core to md
cp /boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot/
cp -r /lib/modules/${Moon_KernelVersion} ${Moon_RootLocation}/lib/modules


## Changes initramfs options to boot using nfs and create initramfs
sed -i 's/^.*BOOT=.*/BOOT=nfs/g' ${Moon_RootLocation}/etc/initramfs-tools/initramfs.conf
mkinitramfs -d ${Moon_RootLocation}/etc/initramfs-tools/ -o ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion}

