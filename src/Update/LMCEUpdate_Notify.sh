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

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

UPDATES_DIR="/home/updates"
XML_OLD="${UPDATES_DIR}/updates.xml.old"
XML_CURRENT="${UPDATES_DIR}/updates.xml"
XML_NEW="${UPDATES_DIR}/updates.xml.new"
XML_LOCK="${UPDATES_DIR}/updates.xml.lock"

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

function Action_Ask() {	
	## AppServer Id for this computer (should only be hybrid)
	Q="
	        SELECT
	                PK_Device
	        FROM
	                Device
	                JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
	        WHERE
	                FK_Device_ControlledVia = $PK_Device
	                AND
	                PK_DeviceTemplate = 26
	
	"
	AppServerID=$(RunSQL "$Q");
	
	Tab=$'\t'
	
	## Sent the Message to the orbiters
	Action_Yes="-targetType device <%=!%> $AppServerID 1 67 13 \"/usr/pluto/bin/LMCEUpdate_Notify.sh\" 51 \"--answer${Tab}yes\""
	Action_No="-targetType device <%=!%> $AppServerID 1 67 13 \"/usr/pluto/bin/LMCEUpdate_Notify.sh\" 51 \"--answer${Tab}no\""
	Action_Later="-targetType device <%=!%> $AppServerID 1 67 13 \"/usr/pluto/bin/LMCEUpdate_Notify.sh\" 51 \"--answer${Tab}later\""
	
	/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 10 "UpdatesNotify" 159 53 9 "New updates available. Would you like to apply the updates the next time you restart your computer ?|Yes|No|Later" 137 "$Action_Yes|$Action_No|$Action_Later" || exit 1
}

function Action_Answer {
	/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 8 10 "UpdatesNotify" 159 53

	local answer="$1"
	case "$answer" in
		"yes")
			Q="Update Device_DeviceData SET IK_DeviceData = '1' WHERE FK_Device='$PK_Device' AND FK_DeviceData='235'"
			RunSQL "$Q"
			cp -r "${XML_CURRENT}" "${XML_OLD}"
			;;
		"no")
			Q="Update Device_DeviceData SET IK_DeviceData = '0' WHERE FK_Device='$PK_Device' AND FK_DeviceData='235'"
			RunSQL "$Q"
			cp -r "${XML_CURRENT}" "${XML_OLD}"
			;;
		"later")
			Q="Update Device_DeviceData SET IK_DeviceData = '0' WHERE FK_Device='$PK_Device' AND FK_DeviceData='235'"
			RunSQL "$Q"
			;;
	esac
}

Ask="true"
while [[ $# -gt 0 ]] ;do
	case "$1" in
		--answer)
			Action_Answer "$2"
			shift
			exit 0
		;;
	esac
done

Action_Ask
