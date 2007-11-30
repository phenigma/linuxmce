#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

set -e

DEVICEDATA_DisklessImages=258
DEVICEDATA_Architecture=112

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

## Install architecture specific kernel
Moon_Architecture=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_Architecture'")
if [[ -z "$Moon_Architecture" ]]; then
	Moon_Architecture=i386
fi
cp /usr/pluto/deb-cache/linux-image-diskless_*_"$Moon_Architecture".deb "$Moon_RootLocation"
pushd "$Moon_RootLocation" >/dev/null
chroot . dpkg -i linux-image-diskless_*_"$Moon_Architecture".deb
popd >/dev/null
rm -f "$Moon_RootLocation"/linux-image-diskless_*_"$Moon_Architecture".deb

## Create symlinks to existing kernels
rm -f ${Moon_RootLocation}/boot/initrd.img
ln -s ${Moon_RootLocation}/boot/initrd.img-${Moon_KernelVersion} ${Moon_RootLocation}/boot/initrd.img
rm -f ${Moon_RootLocation}/boot/vmlinuz
ln -s ${Moon_RootLocation}/boot/vmlinuz-${Moon_KernelVersion} ${Moon_RootLocation}/boot/vmlinuz
