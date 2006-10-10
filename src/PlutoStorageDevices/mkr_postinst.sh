#!/bin/bash

## Create the main mount directory
mkdir -p /mnt/device

## Add a cron entry that scans for Internal Storage Devices
cronEntry="*/10 * * * * root /usr/pluto/bin/StorageDevices_Radar.sh"
if ! grep -qF '/usr/pluto/bin/StorageDevices_Radar.sh' /etc/crontab; then
        echo "$cronEntry" >>/etc/crontab
        /etc/init.d/cron reload
fi

## Add a init script on the core which will look for samba shares over the network
if [[ ! -f /etc/diskless.conf ]] ;then
	ln -s /usr/pluto/bin/StorageDevices_SambaRadar.sh /etc/rc2.d/StorageDevices_SambaRadar.sh
fi
