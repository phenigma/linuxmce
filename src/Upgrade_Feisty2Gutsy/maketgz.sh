#!/bin/bash -e
tgz_root=`pwd`

mkdir -p "${tgz_root}/deb-cache-new"

# CD1 Packages
iso_mountpoint=$(mktemp -d)
mount /var/www/linuxmce-1.1-packages.iso "${iso_mountpoint}" -o loop
cp -v "${iso_mountpoint}/deb-cache/"*.deb                "${tgz_root}/deb-cache-new"
umount "${iso_mountpoint}"
rm -rf "${iso_mountpoint}"

# CD2 Packages
iso_mountpoint=$(mktemp -d)
mount /var/www/linuxmce-1.1-cache.iso "${iso_mountpoint}" -o loop
cp -v "${iso_mountpoint}"/cachecd1-cache/*.deb           "${tgz_root}/deb-cache-new"
umount "${iso_mountpoint}"
rm -rf "${iso_mountpoint}"

# Kubuntu CD Packages
cp -v /var/plutobuild/kubuntu-cd/*.deb                   "${tgz_root}/deb-cache-new"

# AMD64 Packages
cp -v /var/plutobuild/DisklessSync/amd64/deb-cache/*.deb "${tgz_root}/deb-cache-new"


#Diskless Imges
mkdir -p "${tgz_root}/diskless-images"
cp /var/plutobuild/DisklessSync/amd64/PlutoMD-amd64.tar.bz2 "${tgz_root}/diskless-images"
cp /var/plutobuild/DisklessSync/i386/PlutoMD-i386.tar.bz2   "${tgz_root}/diskless-images"
