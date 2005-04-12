#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

PH="plutohome.com"

cronEntry="0 2 * * * root /usr/pluto/bin/Report_Machine_Status.sh"
if ! grep -qF "$cronEntry" /etc/crontab; then
	echo "$cronEntry" >>/etc/crontab
	/etc/init.d/cron reload
fi

Q="SELECT FK_DeviceTemplate
	FROM Device
	WHERE PK_Device='$PK_Device' LIMIT 1"
R=$(RunSQL "$Q")

PK_DeviceTemplate=$(Field 1 "$R")
Uptime=$(awk '{print $1}' /proc/uptime | cut -d. -f1)
Router=$(dpkg -s pluto-dcerouter 2>/dev/null | grep Version | cut -d" " -f2)
Orbiter=$(dpkg -s pluto-orbiter 2>/dev/null | grep Version | cut -d" " -f2)

wget -O /dev/null "http://$PH/report_status.php?Installation=$PK_Installation&Device=$PK_Device&DeviceTemplate=$PK_DeviceTemplate&Uptime=$Uptime&Orbiter=$Orbiter&Router=$Router" &>/dev/null
