#!/bin/bash

set -xe

chmod -R 0755 deb-pkg

PLUTO_FLAVOR="$1"
shift
PLUTO_KERNELS=("$@")
if [[ "${#PLUTO_KERNELS[*]}" -eq 0 ]]; then
    exit 1
fi
PLUTO_PKG_DESTDIR="/home/samba/repositories/"$PLUTO_FLAVOR"/replacements/main/binary-i386/"
    
debDir="$(pwd)/deb-pkg"

KERN_LIST=""
for KERN in "${PLUTO_KERNELS[@]}" ;do
	if [[ "$KERN_LIST" != "" ]] ;then
	    KERN_LIST="${KERN_LIST}|"
	fi
	KERN_LIST="${KERN_LIST}linux-image-$KERN"
done

sed "s/%KERN_LIST%/$KERN_LIST/g" control.in > $debDir/DEBIAN/control

rm -f pluto-kernel-upgrade*.deb
dpkg-deb -b $debDir ./
cp pluto-kernel-upgrade*.deb $PLUTO_PKG_DESTDIR
