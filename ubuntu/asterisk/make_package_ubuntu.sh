#!/bin/bash -x

MAINFOLDER=`pwd`
ADDFOLDER=${MAINFOLDER}/add
TGZFOLDER=${MAINFOLDER}/tgz
SRCFOLDER=${MAINFOLDER}/src
PKGFOLDER=${MAINFOLDER}/pkg/root


rm -rf ${PKGFOLDER} ${SRCFOLDER}


rm -rf asterisk-pluto*
mkdir -p ${PKGFOLDER}/etc
mkdir -p ${SRCFOLDER}

#########
cd ${SRCFOLDER}


for I in `ls ${TGZFOLDER}/*.tar.gz`
do
	echo "Unpacking $I"
	tar -xzf $I
done 


#START COMPILING

cd  ${SRCFOLDER}/app_conference/
make clean
make INSTALL_PREFIX=${PKGFOLDER} || exit
make INSTALL_PREFIX=${PKGFOLDER} install || exit

cd ${SRCFOLDER}/iaxyprov/
make clean
make all
cp iaxyprov ${PKGFOLDER}/usr/sbin/
cp iaxydebug ${PKGFOLDER}/usr/sbin/
cp iaxy.conf.sample ${PKGFOLDER}/etc/asterisk/
strip ${PKGFOLDER}/usr/sbin/iaxyprov
strip ${PKGFOLDER}/usr/sbin/iaxydebug

### ADD AMP TO THIS PACKAGE (PROBABLY NOT BIG ENOUGH)
#patch some stuff
cd ${SRCFOLDER}
patch -p0 < ${ADDFOLDER}/amp_webpage.patch

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

echo "Copy files for build deb packages...."
#copy files needed for dpkg-buildpackage
cp ${ADDFOLDER}/debian-ubuntu/* ${PKGFOLDER}/../debian
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
cp -R ${ADDFOLDER}/extensions.conf ${PKGFOLDER}/etc/asterisk/extensions.conf
cp -R ${ADDFOLDER}/sccp_pluto.conf ${PKGFOLDER}/etc/asterisk/sccp.conf

#copy new amportal
cp ${ADDFOLDER}/amportal ${PKGFOLDER}/usr/sbin/amportal
cp ${ADDFOLDER}/safe_asterisk ${PKGFOLDER}/usr/sbin/safe_asterisk
cp ${ADDFOLDER}/asterisk_keep_running.pl ${PKGFOLDER}/usr/pluto/bin/asterisk_keep_running.pl

#copy AGI scripts
cp -R ${ADDFOLDER}/pluto-sos.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/
cp -R ${ADDFOLDER}/pluto-sos-gen.pl ${PKGFOLDER}/usr/pluto/bin/
cp -R ${ADDFOLDER}/pluto-sos-neighbor.pl ${PKGFOLDER}/usr/pluto/bin/
cp -R ${ADDFOLDER}/pluto-sos-speak.pl ${PKGFOLDER}/usr/pluto/bin/
cp -R ${ADDFOLDER}/pluto-gethousemode.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/
cp -R ${ADDFOLDER}/pluto-getusermode.agi ${PKGFOLDER}/var/lib/asterisk/agi-bin/

#copy Create AMP script (Tuesday, October 17 2006)
#AMP scripts are in src/Asterisk (Wednesday, October 18 2006)
#cp -R ${ADDFOLDER}/create_amp/*.pl ${PKGFOLDER}/usr/pluto/bin/

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


#echo "Compiled successfull.."
# Build packages
dpkg-buildpackage -b -rfakeroot -us -uc

rm -f /tmp/gsmstatus
