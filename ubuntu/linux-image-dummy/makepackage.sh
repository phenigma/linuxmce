#!/bin/bash

Moon_KernelVersion=$(uname -r)
Moon_KernelArch=$(apt-config dump | grep 'APT::Architecture' | sed 's/APT::Architecture "\(.*\)".*/\1/g')
Moon_RootLocation='package/'
mkdir -p $Moon_RootLocation/{boot,lib/modules}

cp /boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot
cp /boot/System.map-${Moon_KernelVersion} ${Moon_RootLocation}/boot/
cp -r /lib/modules/${Moon_KernelVersion} ${Moon_RootLocation}/lib/modules

sed -i 's/^.*BOOT=.*/BOOT=nfs/g' /etc/initramfs-tools/initramfs.conf
mkinitramfs -d /etc/initramfs-tools/ -o ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion}
sed -i 's/^.*BOOT=.*/BOOT=local/g' /etc/initramfs-tools/initramfs.conf


cp ${Moon_RootLocation}/DEBIAN/control{.in,}
sed -i "s/{Kernel_Version}/${Moon_KernelVersion}/g" ${Moon_RootLocation}/DEBIAN/control
sed -i "s/{Kernel_Arch}/${Moon_KernelArch}/g"       ${Moon_RootLocation}/DEBIAN/control

cp -a ${Moon_RootLocation} pkgrootdir
find pkgrootdir -type d -name .svn -exec rm -rf '{}' -prune
cd pkgrootdir
dpkg-deb -b . ..
cd ..
rm -rf pkgrootdir
