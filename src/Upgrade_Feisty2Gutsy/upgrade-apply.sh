#!/bin/bash

set -e
set -x

if [[ ! -f /var/gutsy-upgrade-scripts/upgrade-ready ]] ;then
	exit 0
fi

#cd /usr/share/gutsy-upgrade-scripts/

## Preseed some packages to avoid the need of interactive debconf
./upgrade-preseed.sh

## Put aside some files
mv /etc/apt/sources.list{,.backup-upgrade} || :
mv /etc/init.d/kdm{,.backup-upgrade} || :

## Modify sources.list for the upgade
echo "
deb file:/usr/pluto/deb-cache/ ./
#deb http://archive.ubuntu.com/ubuntu/ gutsy main restricted universe multiverse
" > /etc/apt/sources.list

echo "
Package: *
Pin: origin
Pin-Priority: 9999

Package: *
Pin: release v=7.10,o=Ubuntu,a=gutsy,l=Ubuntu
Pin-Priority: 9998
" > /etc/apt/preferences

## Move deb-cache-new to deb-cache
mv /usr/pluto/deb-cache{,-old}
mv /usr/pluto/deb-cache{-new,}

## Do the actual upgrade
apt-get update
apt-get -f -y --force-yes dist-upgrade

## Fix PK_Distro in pluto.conf
. /usr/pluto/bin/Config_Ops.sh
ConfSet "PK_Distro" "15"

## Fix grub's menu.lst
root_drive=$(mount | grep 'on / ' | cut -d' ' -f1)
sed -i "/^# kopt=/ s|UUID=[a-f0-9-]*|${root_drive}|g" /boot/grub/menu.lst
update-grub

## Reset model device datas
. /usr/pluto/bin/SQL_Ops.sh
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'LMCE_CORE_u0710_i386' WHERE IK_DeviceData = 'LMCE_CORE_1_1'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'LMCE_MD_u0710_i386'   WHERE IK_DeviceData = 'LMCE_MD_1_1'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = '0' WHERE FK_DeviceData = '234'"

## Remove old 0704 updates
rm -rf /home/updates/*

## Regen Packages.gz
pushd /usr/pluto/deb-cache-new
	dpkg-scanpackages -m . /dev/null > Packages
       	gzip -c Packages > Packages.gz
popd


## Don't run again
rm -f /usr/share/gutsy-upgrade-scripts/upgrade-ready

## Don't run download script again
rm -f /etc/cron.d/gutsy-upgrade-scripts
