#!/bin/bash

#init pluto's config
source ./amp_pluto.conf

#create dirs
mkdir -p ./amp_deb/debian

mkdir -p ./amp_deb/root/etc/asterisk
mkdir -p ./amp_deb/root/usr/sbin
mkdir -p ./amp_deb/root/var/lib
mkdir -p ./amp_deb/root/var/spool
mkdir -p ./amp_deb/root/$AMPWEBROOT
mkdir -p ./amp_deb/root/$AMPCGIBIN
#stuff for post install
mkdir -p ./amp_deb/root/usr/local/amp/install/

#copy pluto's config
cp -R ./amp_pluto.conf ./amp_deb/root/etc/amportal.conf

#copy files needed for dpkg-buildpackage
cp ./debian/* ./amp_deb/debian
cp ./Makefile ./amp_deb

#copy what we need in right places
cp -R ./AMP/amp_conf/etc/* ./amp_deb/root/etc/
cp -R ./AMP/amp_conf/usr/sbin/* ./amp_deb/root/usr/sbin/
cp -R ./AMP/amp_conf/var/lib/* ./amp_deb/root/var/lib/
cp -R ./AMP/amp_conf/var/spool/* ./amp_deb/root/var/spool/
cp -R ./AMP/amp_conf/var/www/html/* ./amp_deb/root/$AMPWEBROOT/
cp -R ./AMP/amp_conf/var/www/cgi-bin/* ./amp_deb/root/$AMPCGIBIN/
#stuff for post install
cp ./AMP/SQL/* ./amp_deb/root/usr/local/amp/install/
cp ./AMP/apply_conf.sh ./amp_deb/root/usr/local/amp/install/
cp -R ./AMP/upgrades/ ./amp_deb/root/usr/local/amp/

#patch to change permissions
sed -r -i "s/asterisk[:]asterisk/asterisk:www-data/" ./amp_deb/root/usr/sbin/amportal

cat >> ./amp_deb/root/etc/asterisk/extensions.conf << EOF

; ###########################################################################
; PLUTO's "trusted" context
; ###########################################################################

[trusted]
include => from-internal

EOF

cd ./amp_deb/
find -name '.svn' -type d -exec rm -rf '{}' ';' 2>/dev/null
dpkg-buildpackage -b -rfakeroot
cd ..

