#!/bin/bash

KVER=$(uname -r)
dpkg-buildpackage -rfakeroot -b -us -uc

tar -xzf nvidia-kernel-source.tar.gz
pushd modules/nvidia-kernel/ &>/dev/null
KSRC=/lib/modules/${KVER}/build KVERS=${KVER} fakeroot debian/rules binary_modules
popd &>/dev/null

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2)
cp modules/nvidia-kernel-${KVER}_${Version}_i386.deb ..
