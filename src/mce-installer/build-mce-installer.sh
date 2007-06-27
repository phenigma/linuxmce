#!/bin/bash

pushd src
cp /usr/lib/libvte.so* ./
cp /usr/lib/libglade*so* ./
cp /opt/libsdl1.2-1.2.7+1.2.8cvs20041007/lib/libSDL* ./
cp /usr/lib/libSDL_ttf* ./
cp /usr/lib/libSDL_image* ./
cp /usr/bin/dpkg-scanpackages ./
cp `which xcompmgr` ./

pushd ../../utilities/linux/UIdiag/
	make clean
	make all
popd
cp ../../utilities/linux/UIdiag/UIdiag ./
cp ../../utilities/linux/UIdiag/*.png ./


tar -czf mce-installer.tar.gz mce-installer mce-installer-*.sh mce-installer.glade mce-installer-core.sh mce-installer-preseed.sh mce-installer-diskedmd.sh wget-wrapper.sh pluto.png mce-installer-getnetdata.sh libvte.so* libglade*so* libSDL* mirrors dpkg-scanpackages UIdiag *.png xcompmgr
cat ../mce-installer-header.sh mce-installer.tar.gz > ../mce-installer.sh
rm libvte.so*
rm libglade*so*
rm libSDL*
chmod +x ../mce-installer.sh
rm mce-installer.tar.gz
popd

