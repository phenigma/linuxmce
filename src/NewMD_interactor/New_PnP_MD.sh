#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh

RemoteIP="$1"
RemoteMAC="$2"

Logging "$TYPE" "$SEVERITY_NORMAL" "New PnP MD" "Setting up new PnP MD with MAC address '$RemoteMAC' and provisional IP '$RemoteIP'"

Interactor_Port=7238

DEVICETEMPLATE_Generic_PC_as_MD=28

NewDevice=$(/usr/pluto/bin/CreateDevice -d "$DEVICETEMPLATE_Generic_PC_as_MD" -M "$RemoteMAC" | tail -1)
Logging "$TYPE" "$SEVERITY_NORMAL" "Created MD with Device ID '$NewDevice'"

NewIP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$NewDevice" -a)
Logging "$TYPE" "$SEVERITY_NORMAL" "Assigned permanent IP '$NewIP' to device '$NewDevice'"

WaitLock "NewPnPMD" "NewPnPMD$NewDevice"
Logging "$TYPE" "$SEVERITY_NORMAL" "Running diskless MD setup command suite (device '$NewDevice')"
# Note: keep this in sync with /var/www/pluto-admin/operations/logs/executeLog.php, case 1
/usr/pluto/bin/Diskless_Setup.sh
/usr/pluto/bin/DHCP_config.sh
/usr/pluto/bin/Diskless_ExportsNFS.sh
# End of list to be kept in sync
Logging "$TYPE" "$SEVERITY_NORMAL" "Finished running diskless MD setup command suite (device '$NewDevice')"
Unlock "NewPnPMD" "NewPnPMD$NewDevice"

Logging "$TYPE" "$SEVERITY_NORMAL" "Rebooting new MD"
echo "reboot" | nc -n "$RemoteIP" "$Interactor_Port"
