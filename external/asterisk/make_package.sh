#!/bin/bash

MAINFOLDER=`pwd`
ADDFOLDER=${MAINFOLDER}/add
TGZFOLDER=${MAINFOLDER}/tgz
SRCFOLDER=${MAINFOLDER}/src
PKGFOLDER=${MAINFOLDER}/pkg/root

rm -rf ${PKGFOLDER} ${SRCFOLDER}
rm -rf asterisk-pluto*
mkdir -p ${PKGFOLDER}/etc
mkdir -p ${SRCFOLDER}

cd ${SRCFOLDER}
for I in `ls ${TGZFOLDER}/*.tar.gz`
do
	echo "Unpacking $I"
	tar -xzf $I
done 

chmod u+w -R ${SRCFOLDER}/bristuff-*

#APPLY FLORZ PATCHES
cd ${SRCFOLDER}/bristuff-*
patch -p1 < ${TGZFOLDER}/zaphfc_0.2.0-RC8o_florz-9.diff.gz

#APPLY BRISTUFF PATCHES
cd ${SRCFOLDER}/zaptel-*
echo "Apply patches in `pwd`"
patch -p1 < ${SRCFOLDER}/bristuff-*/patches/zaptel.patch

cd ${SRCFOLDER}/libpri-*
echo "Apply patches in `pwd`"
patch -p1 < ${SRCFOLDER}/bristuff-*/patches/libpri.patch

cd ${SRCFOLDER}/asterisk-[0-9]*
echo "Apply patches in `pwd`"
patch -p1 < ${SRCFOLDER}/bristuff-*/patches/asterisk.patch

#START COMPILING

cd ${SRCFOLDER}/libpri-*
make clean
make all
make INSTALL_PREFIX=${PKGFOLDER}/ install
cd ${PKGFOLDER}/usr/lib
ln -sf libpri.so.1.0 libpri.so.1
ln -sf libpri.so.1 libpri.so

cd ${SRCFOLDER}
ln -s f${SRCFOLDER}/libpri-*/libpri.h ${SRCFOLDER}/asterisk-[0-9]*/include/
ln -sf asterisk-[0-9]* asterisk

cd ${SRCFOLDER}/zaptel-*/
sed -r -i "s/^ROOT_PREFIX=//" Makefile
sed -r -i "s/^INSTALL_PREFIX=//" Makefile
patch < ${ADDFOLDER}/zaptel_2.6.15.patch
touch ${PKGFOLDER}/etc/conf.modules
make clean
make
make INSTALL_PREFIX=${PKGFOLDER} ROOT_PREFIX=${PKGFOLDER} DYNFS=yes install
cp zaptel.conf.sample ${PKGFOLDER}/etc/

cd ${SRCFOLDER}/bristuff-*/
cd qozap
sed -r -i "s/^(BRISTUFFBASE.*)$/\1\/..\//" Makefile
make clean all
make INSTALL_PREFIX=${PKGFOLDER} install
cd ..
cd cwain
sed -r -i "s/^(BRISTUFFBASE.*)$/\1\/..\//" Makefile
make clean all
make INSTALL_PREFIX=${PKGFOLDER} install
cd ..
cd zaphfc
sed -r -i "s/^(BRISTUFFBASE.*)$/\1\/..\//" Makefile
make clean all
make INSTALL_PREFIX=${PKGFOLDER} install

cd ${SRCFOLDER}/spandsp-*/asterisk-*/
mv * ../../asterisk/apps/
cd ${SRCFOLDER}/asterisk-[0-9]*/apps/
patch < apps_makefile.patch

cd ${SRCFOLDER}/spandsp-*/
./configure --prefix=/usr/
make clean
make all
make DESTDIR=${PKGFOLDER} install

cd ${SRCFOLDER}/asterisk-[0-9]*/
make clean
make DESTDIR=${PKGFOLDER} all
make DESTDIR=${PKGFOLDER} install
make DESTDIR=${PKGFOLDER} samples

cd ${SRCFOLDER}/asterisk-addons-*/
make clean
make
make INSTALL_PREFIX=${PKGFOLDER} install

cd  ${SRCFOLDER}/chan_sccp-*/
sed -r -i "s/^INSTALL_PREFIX=//" Makefile
patch < ${ADDFOLDER}/chan_sccp_noask.patch
make clean
make INSTALL_PREFIX=${PKGFOLDER}
make INSTALL_PREFIX=${PKGFOLDER} install
cp conf/sccp.conf ${PKGFOLDER}/etc/asterisk/

cd  ${SRCFOLDER}/app_conference/
make clean
make INSTALL_PREFIX=${PKGFOLDER}
make INSTALL_PREFIX=${PKGFOLDER} install

cd ${SRCFOLDER}/chan_capi-*/
make clean
make INSTALL_PREFIX=${PKGFOLDER}
make INSTALL_PREFIX=${PKGFOLDER} install

### ADD AMP TO THIS PACKAGE (PROBABLY NOT BIG ENOUGH)

cd ${MAINFOLDER}
#init pluto's config
source ${MAINFOLDER}/add/amp_pluto.conf

#create dirs
mkdir -p ${PKGFOLDER}/../debian
mkdir -p ${PKGFOLDER}/etc/asterisk
mkdir -p ${PKGFOLDER}/usr/sbin
mkdir -p ${PKGFOLDER}/usr/pluto/bin
mkdir -p ${PKGFOLDER}/var/lib
mkdir -p ${PKGFOLDER}/var/spool
mkdir -p ${PKGFOLDER}/$AMPWEBROOT
mkdir -p ${PKGFOLDER}/$AMPCGIBIN
mkdir -p ${PKGFOLDER}/var/lib/asterisk/sounds/pluto/
mkdir -p ${PKGFOLDER}/var/run/asterisk/
#stuff for post install
mkdir -p ${PKGFOLDER}/usr/local/amp/install/

#copy files needed for dpkg-buildpackage
cp ${ADDFOLDER}/debian/* ${PKGFOLDER}/../debian
cp ${ADDFOLDER}/Makefile ${PKGFOLDER}/../

#copy what we need in right places
cp -R ${SRCFOLDER}/AMP/amp_conf/etc/*  ${PKGFOLDER}/etc/
cp -R ${SRCFOLDER}/AMP/amp_conf/usr/sbin/*  ${PKGFOLDER}/usr/sbin/
cp -R ${SRCFOLDER}/AMP/amp_conf/var/lib/*  ${PKGFOLDER}/var/lib/
cp -R ${SRCFOLDER}/AMP/amp_conf/var/spool/*  ${PKGFOLDER}/var/spool/
cp -R ${SRCFOLDER}/AMP/amp_conf/var/www/html/*  ${PKGFOLDER}/$AMPWEBROOT/
cp -R ${SRCFOLDER}/AMP/amp_conf/var/www/cgi-bin/*  ${PKGFOLDER}/$AMPCGIBIN/
cp -R ${ADDFOLDER}/pluto-sounds/*  ${PKGFOLDER}/var/lib/asterisk/sounds/pluto/
#stuff for post install
cp ${SRCFOLDER}/AMP/SQL/*  ${PKGFOLDER}/usr/local/amp/install/
cp ${SRCFOLDER}/AMP/apply_conf.sh  ${PKGFOLDER}/usr/local/amp/install/
cp -R ${SRCFOLDER}/AMP/upgrades/  ${PKGFOLDER}/usr/local/amp/
#copy pluto's config
cp -R ${ADDFOLDER}/amp_pluto.conf ${PKGFOLDER}/etc/amportal.conf
cp -R ${ADDFOLDER}/ext_pluto.conf ${PKGFOLDER}/etc/asterisk/extensions_custom.conf
cp -R ${ADDFOLDER}/sccp_pluto.conf ${PKGFOLDER}/etc/asterisk/sccp.conf

#copy new amportal
cp ${ADDFOLDER}/amportal ${PKGFOLDER}/usr/sbin/amportal

#copy AGI scripts
cp -R ${ADDFOLDER}/pluto-sos.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/
cp -R ${ADDFOLDER}/pluto-sos-gen.pl ${PKGFOLDER}/usr/pluto/bin/
cp -R ${ADDFOLDER}/pluto-sos-neighbor.pl ${PKGFOLDER}/usr/pluto/bin/
cp -R ${ADDFOLDER}/pluto-sos-speak.pl ${PKGFOLDER}/usr/pluto/bin/
cp -R ${ADDFOLDER}/pluto-gethousemode.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/
cp -R ${ADDFOLDER}/pluto-getusermode.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/

cat >> ${PKGFOLDER}/etc/asterisk/extensions.conf << EOF

; ###########################################################################
; PLUTOs "trusted" context
; ###########################################################################

[trusted]
include => from-internal

EOF

sed -r -i "s/#include extensions_custom.conf/#include extensions_custom.conf\n#include extensions_pluto_dial.conf/" ${PKGFOLDER}/etc/asterisk/extensions.conf
sed -r -i "s/include => from-internal-custom/include => from-internal-custom\ninclude => from-pluto-custom/" ${PKGFOLDER}/etc/asterisk/extensions.conf
sed -r -i "s/^1234/;1234/" ${PKGFOLDER}/etc/asterisk/voicemail.conf

#don't load SKINNY and CAPI by default
sed -r -i "s/^autoload=yes/autoload=yes\nnoload => chan_skinny.so\nnoload => chan_capi.so\nnoload => app_capiRETRIEVE.so\nnoload => app_capiCD.so\nnoload => app_capiECT.so\nnoload => app_capiFax.so\nnoload => app_capiHOLD.so\nnoload => app_capiMCID.so\nnoload => app_capiNoES.so/" ${PKGFOLDER}/etc/asterisk/modules.conf 

#don't play mp3
sed -r -i "s/^default/;default/" ${PKGFOLDER}/etc/asterisk/musiconhold.conf 

cd ${PKGFOLDER}/../
#make some clean up
find -name '.svn' -type d -exec rm -rf '{}' ';' 2>/dev/null

#strip binaries 
for I in `du -a | grep -E '\.((so)|(a)|(so\..+))$' | cut -f 2`
do
	strip $I
done
strip ${PKGFOLDER}/usr/sbin/asterisk

#removing useless sounds
for I in `du -a | grep '.gsm$' | cut -f 2`
do
	rm -f /tmp/gsmstatus
	
	J=`echo $I | sed -r "s/.+?\/([^\/]+).gsm$/\1/"`
	grep -E -i "(background)|(playback)" ${PKGFOLDER}/etc/asterisk/* | grep "$J" > /tmp/gsmstatus
	J=`echo $I | sed -r "s/.+?\/([^\/]+)$/\1/"`
	grep -E -i "(background)|(playback)" ${PKGFOLDER}/etc/asterisk/* | grep "$J" >> /tmp/gsmstatus
	echo $I | grep -E '(digits)|(letters)|(pluto)' >> /tmp/gsmstatus
	if [ ! -s /tmp/gsmstatus ]
	then
		rm $I	
	fi
done
cp -R ${SRCFOLDER}/asterisk-[0-9]*/sounds/*  ${PKGFOLDER}/var/lib/asterisk/sounds/
rm -f ${PKGFOLDER}/var/lib/asterisk/sounds/*.mp3
rm -f ${PKGFOLDER}/var/lib/asterisk/mohmp3/*
cp ${ADDFOLDER}/short.mp3 ${PKGFOLDER}/var/lib/asterisk/mohmp3/

dpkg-buildpackage -b -rfakeroot -us -uc
