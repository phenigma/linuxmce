#!/bin/bash

Moon_KernelVersion=$(uname -r)
Moon_RootLocation='package/'
mkdir -p $Moon_RootLocation/{boot,lib/modules}

cp /boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot
cp /boot/System.map-${Moon_KernelVersion} ${Moon_RootLocation}/boot/
cp -r /lib/modules/${Moon_KernelVersion} ${Moon_RootLocation}/lib/modules

sed -i 's/^.*BOOT=.*/BOOT=nfs/g' /etc/initramfs-tools/initramfs.conf
mkinitramfs -d /etc/initramfs-tools/ -o ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion}
sed -i 's/^.*BOOT=.*/BOOT=local/g' /etc/initramfs-tools/initramfs.conf

