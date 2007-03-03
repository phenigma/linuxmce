#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0"

## Add a cron entry
cronEntry="*/3 * * * * root /usr/pluto/bin/DiskSpaceMonitor.sh"
if ! grep -qF '/usr/pluto/bin/DiskSpaceMonitor.sh' /etc/crontab; then
        echo "$cronEntry" >>/etc/crontab
        invoke-rc.d cron reload
fi

