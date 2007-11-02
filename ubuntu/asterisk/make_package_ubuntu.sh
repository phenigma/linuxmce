#!/bin/bash -xe

MAINFOLDER=$(pwd)
ADDFOLDER="$MAINFOLDER/add"
TGZFOLDER="$MAINFOLDER/tgz"
SRCFOLDER="$MAINFOLDER/src"
PKGFOLDER="$MAINFOLDER/pkg/root"

PrepareForWork()
{
	rm -rf "$PKGFOLDER" "$SRCFOLDER"

	rm -rf asterisk-pluto*
	mkdir -p "$PKGFOLDER"{/etc/asterisk,/usr/lib/asterisk/modules,/usr/sbin,/../debian,/var/lib/asterisk{/sounds/pluto,/agi-bin},/usr/pluto/bin}
	mkdir -p "$SRCFOLDER"

	#########
	cd "$SRCFOLDER"

	local I
	for I in "$TGZFOLDER"/*.tar.gz; do
		echo "Unpacking $I"
		tar -xzf "$I"
	done 
}

Build_AppConference()
{
	cd "$MAINFOLDER/appconference/"
	make clean
	make ASTERISK_INCLUDE_DIR=../backported/asterisk-1.4.10/include INSTALL_PREFIX="$PKGFOLDER" || exit
	make ASTERISK_INCLUDE_DIR=../backported/asterisk-1.4.10/include INSTALL_PREFIX="$PKGFOLDER" install || exit
}

Build_IaxyProv()
{
	cd "$SRCFOLDER/iaxyprov/"
	make clean
	make all
	cp iaxyprov "$PKGFOLDER/usr/sbin/"
	cp iaxydebug "$PKGFOLDER/usr/sbin/"
	cp iaxy.conf.sample "$PKGFOLDER/etc/asterisk/"
	strip "$PKGFOLDER/usr/sbin/iaxyprov"
	strip "$PKGFOLDER/usr/sbin/iaxydebug"
}

TheRest()
{
	echo "Copy files for build deb packages...."
	#copy files needed for dpkg-buildpackage
	cp ${ADDFOLDER}/debian-ubuntu/* ${PKGFOLDER}/../debian
	cp ${ADDFOLDER}/Makefile ${PKGFOLDER}/../

	cp -R ${ADDFOLDER}/pluto-sounds/*  ${PKGFOLDER}/var/lib/asterisk/sounds/pluto/

	cp -R ${ADDFOLDER}/ext_pluto.conf ${PKGFOLDER}/etc/asterisk/extensions_custom.conf
	cp -R ${ADDFOLDER}/extensions.conf ${PKGFOLDER}/etc/asterisk/extensions.conf
	cp -R ${ADDFOLDER}/sccp_pluto.conf ${PKGFOLDER}/etc/asterisk/sccp.conf

	#copy AGI scripts
	cp -R ${ADDFOLDER}/pluto-sos.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/pluto-sos-gen.pl ${PKGFOLDER}/usr/pluto/bin/
	cp -R ${ADDFOLDER}/pluto-sos-neighbor.pl ${PKGFOLDER}/usr/pluto/bin/
	cp -R ${ADDFOLDER}/pluto-sos-speak.pl ${PKGFOLDER}/usr/pluto/bin/
	cp -R ${ADDFOLDER}/pluto-gethousemode.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/
	cp -R ${ADDFOLDER}/pluto-getusermode.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/

	cat >> ${PKGFOLDER}/etc/asterisk/extensions.conf <<- EOF

	; ###########################################################################
	; PLUTOs "trusted" context
	; ###########################################################################

	[trusted]
	include => from-internal

	EOF

	sed -r -i "s/#include extensions_custom.conf/#include extensions_custom.conf\n#include extensions_pluto_dial.conf/" ${PKGFOLDER}/etc/asterisk/extensions.conf
	sed -r -i "s/include => from-internal-custom/include => from-internal-custom\ninclude => from-pluto-custom/" ${PKGFOLDER}/etc/asterisk/extensions.conf

	cd ${PKGFOLDER}/../

	#strip binaries 
	for I in `du -a | grep -E '\.((so)|(a)|(so\..+))$' | cut -f 2`
	do
		strip $I
	done
}

CreatePackage()
{
	dpkg-buildpackage -b -rfakeroot -us -uc
}

PrepareForWork
Build_AppConference
Build_IaxyProv
TheRest
CreatePackage
