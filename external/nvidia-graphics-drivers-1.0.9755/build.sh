#!/bin/bash

set -ex

if [[ -z "$MakeRelease_Flavor" ]]; then
	echo "ERROR: 'MakeRelease_Flavor' variable not found"
	exit 1
fi

Kernels=("$@")

if [[ "${#Kernels[*]}" -eq 0 ]]; then
	exit 1
fi

DestDir="/home/samba/repositories/"$MakeRelease_Flavor"/"$replacementsdeb"/main/binary-i386/"

KDEPS=
for KVER in "${Kernels[@]}"; do
	KDEPS="$KDEPS, nvidia-kernel-$KVER"
done
KDEPS="$KDEPS" dpkg-buildpackage -rfakeroot -b -us -uc

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2)
#cp ../nvidia-glx{,-dev}_${Version}_i386.deb "$DestDir"

tar -xzf nvidia-kernel-source.tar.gz
pushd modules/nvidia-kernel/ &>/dev/null
for KVER in "${Kernels[@]}"; do
	KSRC=/lib/modules/${KVER}/build KVERS=${KVER} fakeroot debian/rules clean binary_modules
	#cp ../nvidia-kernel-${KVER}_${Version}_i386.deb "$DestDir"
done
popd &>/dev/null
