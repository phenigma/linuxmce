#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting Orbiter"

export DISPLAY=:0

#echo launching out wrapper to launch Mythtv setup if not configured

/usr/pluto/bin/MythTvConfiguredCheck.sh

Q="SELECT PK_Installation FROM Installation LIMIT 1"
installation=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2)

# TODO: support multiple orbiters

Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON PK_Device=PK_Orbiter
WHERE FK_DeviceTemplate=8 AND FK_DeviceCategory=3
AND FK_Installation=$installation"

Orbiters=$(echo "$Q;" | /usr/bin/mysql pluto_main | tail +2 | tr '\n' ' ')

for OrbiterDev in "$Orbiters"; do
	screen -d -m -S Orbiter bash -c "(echo \$(date) Starting; /usr/pluto/bin/Orbiter -d $OrbiterDev -r localhost) | tee Orbiter.log"
	sleep 1
	break
done
