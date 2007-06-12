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

XML_OLD="${UPDATES_DIR}/updates.xml.old"
XML_CURRENT="${UPDATES_DIR}/updates.xml"
XML_NEW="${UPDATES_DIR}/updates.xml.new"
XML_LOCK="${UPDATES_DIR}/updates.xml.lock"


function Action_Ask() {
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
	
	/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 159 53 9 "New updates available. Would you like to install them?|Yes|No" 137 "$Action_Yes|$Action_No" || exit 1
}

function Action_Answer {
	local answer="$1"
	if [[ "$answer" == "yes" ]] ;then
		Q="Update Device_DeviceData SET IK_DeviceData = '1' WHERE FK_Device='$PK_Device' AND FK_DeviceData='235'"
	else
		Q="Update Device_DeviceData SET IK_DeviceData = '0' WHERE FK_Device='$PK_Device' AND FK_DeviceData='235'"
	fi

	RunSQL "$Q"

	## Update the last processed xml
	cp -r "${XML_CURRENT}" "${XML_OLD}"
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
