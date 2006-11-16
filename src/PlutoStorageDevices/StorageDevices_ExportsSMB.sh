#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh

TPL_GENERIC_INTERNAL_DRIVE=1790
DD_BLOCK_DEVICE=152

## Lookup our internal storage devices in the db
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $TPL_GENERIC_INTERNAL_DRIVE AND FK_Device_ControlledVia=$PK_Device"
InternalOwnStorageDevices=$(RunSQL "$Q")

for Device in $InternalOwnStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"
	Device_BlockDevice=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID and FK_DeviceData=$DD_BLOCK_DEVICE")

	Exports_InternalStorageDevices="$Exports_InternalStorageDevices\n
[Storage$Device_ID\$]
comment = Storage Device $Device_ID
browseable = yes
writable = yes
create mask = 0770
directory mask = 0770
path = /mnt/device/$Device_ID
public = yes
guest ok = yes
force user = root
force group = root	
"
done

touch /etc/samba/smb.conf.pluto_storage
PopulateSection "/etc/samba/smb.conf.pluto_storage" "InternalStorageDevices" "$Exports_InternalStorageDevices"

## Check and start/reload the nfs-kernel-server
if [[ "$(pidof smbd)" == "" ]] ;then
	invoke-rc.d samba restart
fi
invoke-rc.d samba reload
