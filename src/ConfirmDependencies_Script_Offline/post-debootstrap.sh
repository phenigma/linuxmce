#!/bin/sh

# replace /target/usr/lib/base-config/menu/pkgsel with ours
cp -f /cdrom/Pluto-Install/pkgsel /target/usr/lib/base-config/menu/

# copy install scripts to /usr/pluto/install
DIR=/target/usr/pluto/install
mkdir -p "$DIR"
SCRIPTS="Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh ConfirmDependencies_Debian.sh Common.sh PreseedStage2.sh preseed.cfg BonusCdMenu.sh Initial_Network_Config.sh Initial_DHCP_Config.sh"
for S in $SCRIPTS; do
	cp -f /cdrom/Pluto-Install/"$S" "$DIR"
done
touch "$DIR/.notdone"

if [ -d /cdrom/Debian-Cache ]; then
	mkdir -p /target/usr/pluto/deb-cache/dists/sarge/main/binary-i386/
	cp -a /cdrom/Debian-Cache/* /target/usr/pluto/deb-cache/dists/sarge/main/binary-i386/ || exit 1
	ln -s sarge /target/usr/pluto/deb-cache/dists/testing
	ln -s sarge /target/usr/pluto/deb-cache/dists/stable
fi

echo 'LANG="en_US.UTF8"
LANGUAGE="en_US:en_GB:en"' >/target/etc/environment

sed -i 's/allow-hotplug/auto/g' /target/etc/network/interfaces

# The installer no longer does this for us, so we have to apply stage 2 preseed values ourselves
chroot /target /usr/pluto/install/PreseedStage2.sh

## Setup pluto's apt.conf
pluto_apt_conf='// Pluto apt conf add-on
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
'
echo -n "$pluto_apt_conf" >/target/etc/apt/apt.conf.d/30pluto
