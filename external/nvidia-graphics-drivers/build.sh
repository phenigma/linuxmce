#!/bin/bash

dpkg-buildpackage -rfakeroot -b -us -uc

tar -xzf nvidia-kernel-source.tar.gz
pushd modules/nvidia-kernel/ &>/dev/null
KSRC=/lib/modules/`uname -r`/build KVERS=`uname -r` fakeroot debian/rules binary_modules
popd &>/dev/null

cp modules/nvidia-kernel-`uname -r`_1.0.8178-2_i386.deb ..
