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

chmod u+w -R ${SRCFOLDER}/bristuff-*

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

cd ${SRCFOLDER}/asterisk-[0-9]*/
make clean
make DESTDIR=${PKGFOLDER} all
make DESTDIR=${PKGFOLDER} install
make DESTDIR=${PKGFOLDER} samples

cd ${SRCFOLDER}/asterisk-addons-*/
make clean
make
make INSTALL_PREFIX=${PKGFOLDER} install

cd  ${SRCFOLDER}/chan_sccp/
sed -r -i "s/^(ASTERISK_SOURCE=).*$/\1\..\/asterisk/" Makefile
sed -r -i "s/^(AST_MODULES_DIR=)(.*)$/\1\$\{INSTALL_PREFIX\}\2/" Makefile
sed -r -i "s/^(ASTERISK_VERSION=).*$/\1v1_0/" Makefile
make clean
make
make INSTALL_PREFIX=${PKGFOLDER} install
cp conf/sccp.conf ${PKGFOLDER}/etc/asterisk/

cd ${SRCFOLDER}/chan_capi-*/
make clean
make INSTALL_PREFIX=${PKGFOLDER}
make INSTALL_PREFIX=${PKGFOLDER} install

#don't load by default these modules
sed -r -i "s/^autoload=yes/autoload=yes\nnoload => app_conference.so\nnoload => chan_capi.so\nnoload => app_capiRETRIEVE.so\nnoload => app_capiCD.so\nnoload => app_capiECT.so\nnoload => app_capiFax.so\nnoload => app_capiHOLD.so\nnoload => app_capiMCID.so\nnoload => app_capiNoES.so/" ${PKGFOLDER}/etc/asterisk/modules.conf 

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
#stuff for post install
mkdir -p ${PKGFOLDER}/usr/local/amp/install/

#copy pluto's config
cp -R ${ADDFOLDER}/amp_pluto.conf ${PKGFOLDER}/etc/amportal.conf
cp -R ${ADDFOLDER}/ext_pluto.conf ${PKGFOLDER}/etc/asterisk/extensions_custom.conf

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

#patch amportal script
sed -r -i "s/asterisk[:]asterisk/asterisk:www-data/" ${PKGFOLDER}/usr/sbin/amportal
sed -r -i "s/chmod u[+]x/chmod ug+x/" ${PKGFOLDER}/usr/sbin/amportal
sed -r -i "s/&& export LD_LIBRARY_PATH=\/usr\/local\/lib/ /" ${PKGFOLDER}/usr/sbin/amportal

#copy AGI scripts
cp -R ${ADDFOLDER}/pluto-sos.agi  ${PKGFOLDER}/var/lib/asterisk/agi-bin/
cp -R ${ADDFOLDER}/pluto-sos-gen.pl  ${PKGFOLDER}/usr/pluto/bin/

cat >> ${PKGFOLDER}/etc/asterisk/extensions.conf << EOF

; ###########################################################################
; PLUTOs "trusted" context
; ###########################################################################

[trusted]
include => from-internal

EOF

cd ${PKGFOLDER}/../
find -name '.svn' -type d -exec rm -rf '{}' ';' 2>/dev/null
dpkg-buildpackage -b -rfakeroot
