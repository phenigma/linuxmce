#!/bin/bash


## Add the /mnt/device into auto.master (for automount) and create the /mnt/device dir
AutoMaster_StorageDevices="/mnt/device /etc/auto.PlutoStorageDevices --timeout=10"
PopulateSection "/etc/auto.master" "PlutoStorageDevices" "$AutoMaster_StorageDevices"
mkdir -p /mnt/device

## Add a cron entry
cronEntry="*/10 * * * * root /usr/pluto/bin/StorageDevices_Radar.sh"
if ! grep -qF '/usr/pluto/bin/StorageDevices_Radar.sh' /etc/crontab; then
        echo "$cronEntry" >>/etc/crontab
        /etc/init.d/cron reload
fi

