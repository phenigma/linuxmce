#!/bin/bash

set -e
set -x

## Preseed some backages to avoid the need of interactive debconf
./upgrade-preseed.sh

## Put aside some files
mv /etc/apt/sources.list{,.backup-upgrade} || :
mv /etc/init.d/kdm{,.backup-upgrade} || :

## Modify sources.list for the upgade
echo "
deb file:/usr/pluto/deb-cache/ ./
deb http://archive.ubuntu.com/ubuntu/ gutsy main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ gutsy-security main restricted universe multiverse
deb http://archive.ubuntu.com/ubuntu/ gutsy-updates  main restricted universe multiverse
" > /etc/apt/sources.list

## Do the actual upgrade
apt-get update
apt-get -f -y --force-yes dist-upgrade

## Fix grub's menu.lst
root_drive=$(mount | grep 'on / ' | cut -d' ' -f1)
sed -i "/^# kopt=/ s|UUID=[a-f0-9-]*|${root_drive}|g" /boot/grub/menu.lst
update-grub

## Reset model device datas
. /usr/pluto/bin/SQL_Ops.sh
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'LMCE_CORE_u0710_i386' WHERE IK_DeviceData = 'LMCE_CORE_1_1'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = 'LMCE_MD_u0710_i386'   WHERE IK_DeviceData = 'LMCE_MD_1_1'"
RunSQL "UPDATE Device_DeviceData SET IK_DeviceData = '0' WHERE FK_DeviceData = '234'"
