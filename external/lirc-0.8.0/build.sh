#!/bin/bash

KVER=$(uname -r)
KVER=${KVER%-smp}
dpkg-buildpackage -rfakeroot -b -us -uc

rm -rf modules
tar -xzf debian/lirc-modules-source/usr/src/lirc-modules.tar.gz
pushd modules/lirc/ &>/dev/null
KSRC=/lib/modules/${KVER}/build KVERS=${KVER} fakeroot debian/rules binary-modules
KSRC=/lib/modules/${KVER}-smp/build KVERS=${KVER}-smp fakeroot debian/rules binary-modules
popd &>/dev/null

Version=$(dpkg-parsechangelog -l./debian/changelog |grep ^Version|cut -d' ' -f2)
cp modules/lirc-modules-${KVER}{,-smp}_${Version}_i386.deb ..
