#!/bin/bash

pushd src
cp /usr/lib/libvte.so* ./
cp /usr/lib/libglade*so* ./
cp /opt/libsdl1.2-1.2.7+1.2.8cvs20041007/lib/libSDL*
cp /usr/bin/dpkg-scanpackages ./


pushd ../../utilities/linux/UIdiag/
	make clean
	make all
popd
cp ../../utilities/linux/UIdiag/UIdiag ./


tar -czf mce-installer.tar.gz mce-installer mce-installer-*.sh mce-installer.glade mce-installer-core.sh mce-installer-preseed.sh mce-installer-diskedmd.sh wget-wrapper.sh pluto.png mce-installer-getnetdata.sh libvte.so* libglade*so* libSDL* mirrors dpkg-scanpackages UIdiag
cat ../mce-installer-header.sh mce-installer.tar.gz > ../mce-installer.sh
rm libvte.so*
rm libglade*so*
rm libSDL*
chmod +x ../mce-installer.sh
rm mce-installer.tar.gz
popd

