#!/bin/bash

if ! grep -q "silent_modules" /etc/kernel-img.conf; then
	# should we write the entire file just to be sure?
	echo "silent_modules = yes" >>/etc/kernel-img.conf
fi

echo "Copying kernel package(s) for later use"
cp -v /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null || /bin/true
