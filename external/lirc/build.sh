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
	KDEPS="$KDEPS, linux-image-$KVER"
done

fakeroot debian/rules clean || :
for KVER in "${Kernels[@]}"; do
	KSRC=/lib/modules/${KVER}/build KVERS=${KVER} fakeroot debian/rules build-modules
done
KDEPS="$KDEPS" fakeroot debian/rules binary

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2)
#cp ../{lirc{,-{x,svga}},liblircclient{0,-dev}}_${Version}_i386.deb "$DestDir"
cp ../lirc-pluto_${Version}_i386.deb "$DestDir"
