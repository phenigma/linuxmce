#!/bin/bash
. /usr/pluto/bin/Utils.sh

## Remove fuppes web bookmark
DelBookmark "http://dcerouter:3877/"

## Remove the cron entry
#cat  /etc/crontab  | grep -v "StorageDevices_Radar.sh" > /etc/crontab.$$
#mv /etc/crontab.$$ /etc/crontab
rm -f /etc/cron.d/StorageDevicesRadar

## Remove the init script that does the samba server scanning
rm -f /etc/rc2.d/StorageDevices_SambaRadar.sh
rm -f /etc/init.d/StorageDevices_SambaRadar.sh

## Remove the init script that does the status (online/offline) scanning
rm -f /etc/rc2.d/StorageDevices_StatusRadar.sh
rm -f /etc/init.d/StorageDevices_StatusRadar.sh

## Remove umount diversion
dpkg-divert --remove /bin/umount
dpkg-divert --remove /bin/mount
