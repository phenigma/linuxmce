#!/bin/bash
# 
# Description:
#   Notifies via an orbiter screen that a certain device has some updates
#   available and needs a reboot to apply them.
#
# Usage:
#   NotifyUpdates.sh <DeviceID>
#
# Changelog:
#   * Initial Release
#  -- Razvan Gavril <razvan.g@plutohome.com>  Tue, 07 Mar 2006 16:45:50 +0200
#

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

function ShowUsage {
	echo "Usage : NotifyUpdates.sh <DeviceID>"
	echo 
	echo "Notifies via an orbiter screen that a certain device has some updates"
  	echo "available and needs a reboot to apply them."
	echo
}

## Read the device to be rebooted id
if [[ $# != 1 ]]; then
	ShowUsage
	exit 1
fi
DeviceToRebootID=$1

## Generate a list containig all orbiter ids ($OrbiterIDList)
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

## AppServer Id for the device to be rebooted
Q="
        SELECT
                PK_Device
        FROM
                Device
                JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
        WHERE
                FK_Device_ControlledVia = $DeviceToRebootID
                AND
                PK_DeviceTemplate = 26

"
DeviceToRebootAppServerID=$(RunSQL "$Q");


## Sent the Message to the orbiters
/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 10 "" 159 210 2 $DeviceToRebootID 192 $DeviceToRebootAppServerID
