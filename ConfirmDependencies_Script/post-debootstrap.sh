#!/bin/sh

# replace /target/usr/lib/base-config/menu/pkgsel with ours
cp -f /cdrom/Pluto-Install/pkgsel /target/usr/lib/base-config/menu/

# copy install scripts to /usr/pluto/install
DIR=/target/usr/pluto/install
mkdir -p "$DIR"
SCRIPTS="ConfirmDependencies_Debian.sh Initial_Config.sh Common.sh Download_Direct.sh Download_SVN.sh Download_CVS.sh"
for S in $SCRIPTS; do
	cp -f /cdrom/Pluto-Install/"$S" "$DIR"
done
touch "$DIR/.notdone"

if [ -d /cdrom/Debian-Cache ]; then
	mkdir -p /target/usr/pluto/install/deb-cache
	cp -a /cdrom/Debian-Cache/* /target/usr/pluto/install/deb-cache
fi
