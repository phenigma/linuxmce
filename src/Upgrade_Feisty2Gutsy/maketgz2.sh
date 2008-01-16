#!/bin/bash -ex
tgz_root=`pwd`

mkdir -p "${tgz_root}/deb-cache-new"
# CD1 Packages
iso_mountpoint=$(mktemp -d)
mount ../ISO/LinuxMCE-CD1.iso "${iso_mountpoint}" -o loop
cp -v "${iso_mountpoint}/deb-cache/"*.deb "${tgz_root}/deb-cache-new"
umount "${iso_mountpoint}"
rm -rf "${iso_mountpoint}"

# CD2 Packages
iso_mountpoint=$(mktemp -d)
mount ../ISO/LinuxMCE-CD2.iso "${iso_mountpoint}" -o loop
cp -v "${iso_mountpoint}"/cachecd1-cache/*.deb "${tgz_root}/deb-cache-new"
umount "${iso_mountpoint}"
rm -rf "${iso_mountpoint}"

# Kubuntu CD Packages
cp -v /var/lmce-build/kubuntu-cd/*.deb "${tgz_root}/deb-cache-new"

mkdir -p "${tgz_root}/diskless-images"
iso_mountpoint=$(mktemp -d)
mount ../ISO/LinuxMCE-DVD-i386.iso "${iso_mountpoint}" -o loop
cp -v ${iso_mountpoint}/deb-cache/*.deb "${tgz_root}/deb-cache-new"
cp -v ${iso_mountpoint}/diskless-images/PlutoMD-*.tar.bz2 "${tgz_root}/diskless-images"
umount "${iso_mountpoint}"
rm -rf "${iso_mountpoint}"

