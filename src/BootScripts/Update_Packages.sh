#!/bin/bash

echo "Performing system update"
apt-get update
apt-get -y dist-upgrade
cp /var/cache/apt/archives/kernel-image* /usr/pluto/deb-cache/dists/sarge/main/binary-i386/ 2>/dev/null
apt-get clean
