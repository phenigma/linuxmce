#!/bin/bash

pushd src
cp /usr/lib/libvte.so* ./
cp /usr/lib/libglade*so* ./
tar -czf mce-installer.tar.gz mce-installer mce-installer-UbuntuCD*.sh mce-installer.glade mce-installer-core.sh mce-installer-preseed.sh mce-installer-diskedmd.sh wget-wrapper.sh pluto.png mce-installer-getnetdata.sh libvte.so* libglade*so* mirrors
cat ../mce-installer-header.sh mce-installer.tar.gz > ../mce-installer.sh
rm libvte.so*
rm libglade*so*
chmod +x ../mce-installer.sh
rm mce-installer.tar.gz
popd

