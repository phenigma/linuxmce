#!/bin/bash

set -e

exec 100>&1
exec >"/var/log/upgrade-apply.log"
exec 2>&1

function DisplayMessage {
        echo "`date +%H:%M:%S`  $*" >&100
}

function Error {
        set +x
        trap - EXIT
        echo
        DisplayMessage "ERROR: $*"
        echo                    		>&100
        echo     		              	>&100
        tail -20 /var/log/upgrade-apply.log	>&100
        echo                    		>&100

        exit 1
}

trap 'Error "Got a error in file $0"' EXIT
set -x

cd /usr/share/gutsy-upgrade-scripts/scripts

## Preseed some packages to avoid the need of interactive debconf
DisplayMessage "Preseeding packages"
./upgrade-preseed.sh

## Regen Packages.gz
DisplayMessage "Setting up deb-cache"
apt-get -f -y --force-yes install dpkg-dev

## Move deb-cache-new to deb-cache
if [[ ! -d /usr/pluto/deb-cache-old ]] ;then
	mv /usr/pluto/deb-cache{,-old} 
fi

if [[ -d /usr/pluto/deb-cache-new ]] ;then
	mv /usr/pluto/deb-cache{-new,}
fi

pushd /usr/pluto/deb-cache
	dpkg-scanpackages -m . /dev/null > Packages
       	gzip -c Packages > Packages.gz
popd

## Put aside some files
mv /etc/apt/sources.list{,.backup-upgrade} || :
mv /etc/init.d/kdm{,.backup-upgrade} || :

## Modify sources.list for the upgade
DisplayMessage "Setting up apt"
echo "
deb file:/usr/pluto/deb-cache/ ./
deb http://archive.ubuntu.com/ubuntu/ gutsy main restricted universe multiverse
" > /etc/apt/sources.list

## Setting up apt
echo "
Package: *
Pin: origin
Pin-Priority: 9999

Package: *
Pin: release v=7.10,o=Ubuntu,a=gutsy,l=Ubuntu
Pin-Priority: 9998
" > /etc/apt/preferences


## Do the actual upgrade
DisplayMessage "Started updating packages"
sleep 2
apt-get update
apt-get -f -y --force-yes install mysql-server-5.0
/etc/init.d/mysql start
apt-get -f -y --force-yes dist-upgrade

## Fix PK_Distro in pluto.conf
DisplayMessage "Performing post upgrade fixes"
. /usr/pluto/bin/Config_Ops.sh
ConfSet "PK_Distro" "15"

## Fix grub's menu.lst
root_drive=$(mount | grep 'on / ' | cut -d' ' -f1)
sed -i "/^# kopt=/ s|UUID=[a-f0-9-]*|${root_drive}|g" /boot/grub/menu.lst
update-grub

## Reset model device datas
. /usr/pluto/bin/SQL_Ops.sh
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = '15' WHERE PK_Device IN (SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN (7, 28)) AND FK_DeviceData='7'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'LMCE_CORE_u0710_i386' WHERE IK_DeviceData = 'LMCE_CORE_1_1'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'LMCE_MD_u0710_i386'   WHERE IK_DeviceData = 'LMCE_MD_1_1'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'i386' WHERE FK_DeviceData = '112'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = '0' WHERE FK_DeviceData = '234'"

# Put kdm startup script back
mv /etc/init.d/kdm{.backup-upgrade,} || :

## Remove old 0704 updates
rm -rf /home/updates/*

## Don't run again
rm -f /usr/share/gutsy-upgrade-scripts/upgrade-ready

## Don't run download script again
DisplayMessage "Upgrade Finished"
rm -f /etc/cron.d/gutsy-upgrade-scripts

apt-get -f -y --force-yes --purge remove gutsy-upgrade-scripts
trap - EXIT

