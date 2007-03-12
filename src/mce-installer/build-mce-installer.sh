#!/bin/bash

pushd src
tar -czf mce-installer.tar.gz mce-installer mce-installer.glade mce-installer-core.sh mce-installer-preseed.sh mce-installer-diskedmd.sh wget-wrapper.sh pluto.png mce-installer-getnetdata.sh
cat ../mce-installer-header.sh mce-installer.tar.gz > ../mce-installer.sh
chmod +x ../mce-installer.sh
rm mce-installer.tar.gz
popd

