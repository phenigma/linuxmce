#!/bin/bash

KVER=$(uname -r)
KVER=${KVER%-smp}
dpkg-buildpackage -rfakeroot -b -us -uc

tar -xzf nvidia-kernel-source.tar.gz
pushd modules/nvidia-kernel/ &>/dev/null
KSRC=/lib/modules/${KVER}/build KVERS=${KVER} fakeroot debian/rules clean binary_modules
KSRC=/lib/modules/${KVER}-smp/build KVERS=${KVER}-smp fakeroot debian/rules clean binary_modules
popd &>/dev/null

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2)
cp modules/nvidia-kernel-${KVER}{,-smp}_${Version}_i386.deb ..
