#!/bin/bash

. /usr/pluto/install/Common.sh

# XXX: Setup Bonus CD
echo "The Pluto Bonus CD version 1 has some extras, such as a video setup"
echo "wizard to help get you up and running. If you have the Pluto Bonus CD"
echo "version 1, please insert it into your drive now and choose Y after it is in."
echo "Otherwise, choose N."
echo ""
BonusCD=$(Ask "Did you insert the \"Pluto Bonus CD 1\" in drive ? [y/N]")
if [[ "$BonusCD" == Y || "$BonusCD" == y ]]; then
	if [ ! -d /cdrom ]; then
		mkdir /cdrom
	fi
	/bin/mount /dev/cdrom /cdrom 2>/dev/null

	while [ ! -d "/cdrom/bonuscd1" ]; do
		echo "This in not a valid \"Pluto Bonus CD 1\". Please insert the correct CD and try again."
		/usr/bin/eject
		echo "Press any key when you inserted the correct CD in drive."
		read key

		if [[ ! -n "$key" ]]; then
			/bin/mount /dev/cdrom /cdrom 2>/dev/null
		fi
	done
	echo "Installing extra packages from \"Pluto Bonus CD 1\""
	echo "... PLEASE WAIT ..."

	BonusAutoInstDir=/usr/pluto/deb-cache/auto-inst
	mkdir -p "$BonusAutoInstDir"
	cp /cdrom/bonuscd1/*.deb "$BonusAutoInstDir"

	BonusWorkDir="/usr/pluto/deb-cache/dists/sarge/main/binary-i386"

	cd /cdrom/bonuscd1-cache
	cp -r *.deb "$BonusWorkDir"
	cd /cdrom/bonuscd1-offline
	cp -r *.deb "$BonusWorkDir"
	cd "$BonusWorkDir"
	dpkg -i libsepol1_*.deb 1>/dev/null 2>/dev/null
	dpkg -i libselinux1_*.deb 1>/dev/null 2>/dev/null
	dpkg -i binutils_*.deb 1>/dev/null 2>/dev/null
	dpkg -i patch_*.deb 1>/dev/null 2>/dev/null
	dpkg -i dpkg_*.deb 1>/dev/null 2>/dev/null
	dpkg -i dpkg-dev_*.deb 1>/dev/null 2>/dev/null

	cd "$BonusWorkDir"
	echo "Regenerating Packages.gz in debcache. This will require 1 minute. Please wait."
	dpkg-scanpackages . /dev/null | sed 's,\./,dists/sarge/main/binary-i386/,g' | gzip -9c > Packages.gz
	clear
	cd ..
	/usr/bin/eject
	echo ""
	echo "\"Pluto Bonus CD 1\" succesfuly installed !"
	echo ""
else
	echo "Skipping \"Pluto Bonus CD 1\" install ..."
fi
