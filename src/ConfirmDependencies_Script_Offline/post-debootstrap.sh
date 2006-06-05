#!/bin/sh

# replace /target/usr/lib/base-config/menu/pkgsel with ours
cp -f /cdrom/Pluto-Install/pkgsel /target/usr/lib/base-config/menu/

# copy install scripts to /usr/pluto/install
DIR=/target/usr/pluto/install
mkdir -p "$DIR"
SCRIPTS="Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh ConfirmDependencies_Debian.sh Common.sh PreseedStage2.sh preseed.cfg"
for S in $SCRIPTS; do
	cp -f /cdrom/Pluto-Install/"$S" "$DIR"
done
touch "$DIR/.notdone"

if [ -d /cdrom/Debian-Cache ]; then
	mkdir -p /target/usr/pluto/deb-cache/dists/sarge/main/binary-i386/
	cp -a /cdrom/Debian-Cache/* /target/usr/pluto/deb-cache/dists/sarge/main/binary-i386/
	ln -s sarge /target/usr/pluto/deb-cache/dists/testing
	ln -s sarge /target/usr/pluto/deb-cache/dists/stable
fi

echo 'LANG="en_US.UTF8"
LANGUAGE="en_US:en_GB:en"' >/target/etc/environment

sed -i 's/allow-hotplug/auto/g' /target/etc/network/interfaces

# The installer no longer does this for us, so we have to apply stage 2 preseed values ourselves
chroot /target /usr/pluto/install/PreseedStage2.sh
