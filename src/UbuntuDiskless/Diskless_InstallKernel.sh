#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh

set -e

DEVICEDATA_DisklessImages=258

if [[ "$#" -ne 1 ]]; then
	echo "Syntax: $0 <Device>"
	exit 1
fi
Moon_DeviceID="$1"
Moon_RootLocation="/usr/pluto/diskless/${Moon_DeviceID}"

DisklessImages=$(GetDeviceData "$Moon_DeviceID" "$DEVICEDATA_DisklessImages")
if [[ -n "$DisklessImages" ]]; then
	exit 0
fi

## Determine what kernel are we going to use
#Moon_KernelVersion=$(basename $(ls /boot/vmlinuz-* | head -1) | cut -d"-" -f2-99)
Moon_KernelVersion=$(uname -r)

## Move kernel from core to md
cp /boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot/
cp /boot/System.map-${Moon_KernelVersion} ${Moon_RootLocation}/boot/
cp -r /lib/modules/${Moon_KernelVersion} ${Moon_RootLocation}/lib/modules

## Next line will act as a diversion remove for hd-dvd-ripping bla bla
mv -f ${Moon_RootLocation}/lib/modules/${Moon_KernelVersion}/kernel/fs/udf/udf.ko{.old,}


## Changes initramfs options to boot using nfs and create initramfs
sed -i 's/^.*BOOT=.*/BOOT=nfs/g' ${Moon_RootLocation}/etc/initramfs-tools/initramfs.conf
mkinitramfs -d ${Moon_RootLocation}/etc/initramfs-tools/ -o ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion}

## Create symlinks to existing kernels
rm -f ${Moon_RootLocation}/boot/initrd.img
ln -s ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion} ${Moon_RootLocation}/boot/initrd.img
rm -f ${Moon_RootLocation}/boot/vmlinuz
ln -s ${Moon_RootLocation}/boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot/vmlinuz
