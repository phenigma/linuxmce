#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting Bluetooth Dongle"

Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE FK_DeviceTemplate=13
AND FK_Installation=$installation"

Dongles=$(echo "$Q;" | /usr/bin/mysql pluto_main)

for Dongle in $Dongles; do
	screen -d -m -S Bluetooth bash -c "(echo \$(date) Starting; /usr/pluto/bin/Bluetooth_Dongle -d $Dongle -r localhost) | tee Bluetooth.log"
	sleep 1
	break
done
