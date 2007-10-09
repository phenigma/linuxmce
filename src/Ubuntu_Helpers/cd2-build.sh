#!/bin/bash
iso_name="$1"
if [[ "$iso_name" == "" ]] ;then
	iso_name="linuxmce-1.1"
fi

rm -f "/var/www/${iso_name}-cache.iso"

pushd /var
mkisofs -f -J -r -o "${iso_name}-cache.iso" ./ubuntu
mv "${iso_name}-cache.iso" ./www/
popd
