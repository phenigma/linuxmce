#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh

Interactor_Port=7238

RemoteIP="$1"
RemoteMAC="$2"
Room="$3"

FeedbackMD()
{
	echo "msg $1" | nc -n -q 0 "$RemoteIP" "$Interactor_Port"
}

Logging "$TYPE" "$SEVERITY_NORMAL" "New PnP MD" "Setting up new PnP MD with MAC address '$RemoteMAC' and provisional IP '$RemoteIP'"

DEVICETEMPLATE_Generic_PC_as_MD=28

NewDevice=$(/usr/pluto/bin/CreateDevice -d "$DEVICETEMPLATE_Generic_PC_as_MD" -M "$RemoteMAC" | tail -1)
Logging "$TYPE" "$SEVERITY_NORMAL" "Created MD with Device ID '$NewDevice'"
FeedbackMD "Created MD with Device ID '$NewDevice'"

NewIP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$NewDevice" -a)
Logging "$TYPE" "$SEVERITY_NORMAL" "Assigned permanent IP '$NewIP' to device '$NewDevice'"
FeedbackMD "Assigned permanent IP '$NewIP' to device '$NewDevice'"

# Create room
sqlRoom="${Room//\'/\'}"
Q="INSERT INTO Room(FK_Installation, FK_RoomType, Description)
	VALUES(1, 9, '$sqlRoom');
	SELECT LAST_INSERT_ID()"
NewRoom=$(RunSQL "$Q")
Logging "$TYPE" "$SEVERITY_NORMAL" "Created new room '$Room' with ID '$NewRoom'"
FeedbackMD "Created new room '$Room' with ID '$NewRoom'"

# Put machine in created room
Q="UPDATE Device SET FK_Room='$NewRoom' WHERE PK_Device='$NewDevice'"
RunSQL "$Q"
Logging "$TYPE" "$SEVERITY_NORMAL" "Added new MD '$NewDevice' to room '$Room' ($NewRoom)"
FeedbackMD "Added new MD '$NewDevice' to room '$Room' ($NewRoom)"

WaitLock "NewPnPMD" "NewPnPMD$NewDevice"
Logging "$TYPE" "$SEVERITY_NORMAL" "Running diskless MD setup command suite (device '$NewDevice')"
# Note: keep this in sync with /var/www/pluto-admin/operations/logs/executeLog.php, case 1
FeedbackMD "Running Diskless_Setup.sh"
/usr/pluto/bin/Diskless_Setup.sh
FeedbackMD "Running DHCP_config.sh"
/usr/pluto/bin/DHCP_config.sh
FeedbackMD "Running Diskless_ExportsNFS.sh"
/usr/pluto/bin/Diskless_ExportsNFS.sh
# End of list to be kept in sync
Logging "$TYPE" "$SEVERITY_NORMAL" "Finished running diskless MD setup command suite (device '$NewDevice')"
Unlock "NewPnPMD" "NewPnPMD$NewDevice"

Logging "$TYPE" "$SEVERITY_NORMAL" "Rebooting new MD"
echo "reboot" | nc -n "$RemoteIP" "$Interactor_Port"
