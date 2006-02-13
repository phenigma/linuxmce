#!/bin/bash

tar -xzf nvidia-kernel-source.tar.gz
pushd modules/nvidia-kernel/ &>/dev/null
KSRC=/lib/modules/`uname -r`/build KVERS=`uname -r` fakeroot debian/rules binary_modules
popd &>/dev/null

cp modules/nvidia-kernel/nvidia-kernel-`uname -r`_1.0.8178-1_i386.deb
