#!/bin/bash

## Add a cron entry
cronEntry="*/10 * * * * root /usr/pluto/bin/DiskSpaceMonitor.sh"
if ! grep -qF '/usr/pluto/bin/DiskSpaceMonitor.sh' /etc/crontab; then
        echo "$cronEntry" >>/etc/crontab
        /etc/init.d/cron reload
fi

