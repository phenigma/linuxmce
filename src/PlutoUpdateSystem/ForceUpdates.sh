#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

## Kill all DownloadUpdates.sh scripts that are running
DownloadUpdates_PIDS=$(pidof DownloadUpdates.sh)
if [[ "$DownloadUpdates_PIDS" != "" ]]; then
	for pid in $DownloadUpdates_PIDS ;do
		kill $pid
		wait $pid
	done
fi

## If onther ForceUpdates.sh instances are running, kill them too
ForceUpdates_PIDS=$(pidof ForceUpdates.sh)
if [[ "$ForceUpdates_PIDS" != "" ]]; then
	for pid in $ForceUpdates_PIDS ;do
		if [[ "$pid" != "$$" ]]; then
			kill $pid
			wait $pid
		fi
	done
fi

## Go ahead, force that upgrade
UpdatesOkStamp=$(date +%s)
touch /usr/pluto/var/Updates/UpdatesOk.flag
echo $UpdatesOkStamp > /usr/pluto/var/Updates/UpdatesOk.flag
echo $UpdatesOkStamp > /usr/pluto/var/Updates/ForceUpdates.stamp

## Generate a list containig all orbiter ids ($OrbiterIDList) to display the reboot message
Q="
        SELECT
                PK_Device
        FROM
                Device
                JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
                JOIN DeviceCategory ON PK_DeviceCategory = FK_DeviceCategory
        WHERE
                PK_DeviceCategory IN (5,2,3)
"

OrbiterList=$(RunSQL "$Q")

OrbiterIDList=""
for Orbiter in $OrbiterList; do
        if [[ $OrbiterIDList != "" ]]; then
                OrbiterIDList="$OrbiterIDList,"
        fi

        Orbiter_ID=$(Field 1 "$Orbiter")
        OrbiterIDList="$OrbiterIDList""$Orbiter_ID"
done

## Force the reboot
/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList  1 741 159 53 9 'Restarting the Core to force a system update ...' 163 "reboot" 183 "0"
reboot
