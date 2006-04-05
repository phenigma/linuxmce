#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh

TPL_GENERIC_INTERNAL_DRIVE=1790
DD_BLOCK_DEVICE=152

## Add the /mnt/device into auto.master (for automount)
AutoMaster_StorageDevices="/mnt/device /etc/auto.PlutoStorageDevices --timeout=0,--ghost"
PopulateSection "/etc/auto.master" "PlutoStorageDevices" "$AutoMaster_StorageDevices"
mkdir -p /mnt/device

## Lookup our internal storage devices in the db
Q="SELECT PK_Device, Description  FROM Device WHERE FK_DeviceTemplate = $TPL_GENERIC_INTERNAL_DRIVE AND FK_Device_ControlledVia=$PK_Device"
InternalOwnStorageDevices=$(RunSQL "$Q")

for Device in $InternalOwnStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_Description=$(Field 2 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"
	Device_BlockDevice=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_BLOCK_DEVICE")
	
	#linux          -ro,soft,intr           ftp.example.org:/pub/linux
	Fstab_InternalStorageDevices="$Fstab_InternalStorageDevices\n$Device_ID -fstype=auto :$Device_BlockDevice"
	#Fstab_InternalStorageDevices="$Fstab_InternalStorageDevices\n$Device_BlockDevice $Device_MountPoint auto defaults 0 0"
done

PopulateSection "/etc/auto.PlutoStorageDevices" "InternalStorageDevices" "$Fstab_InternalStorageDevices"


## Lookup for other computers storage devices and add them to fstab
## as nfs shares
Q="
SELECT
	Drive.PK_Device,
	Host.IPaddress
FROM
	Device Drive
	JOIN Device Host ON Host.PK_Device = Drive.FK_Device_ControlledVia
WHERE 
	Drive.FK_DeviceTemplate = $TPL_GENERIC_INTERNAL_DRIVE
	AND
	Drive.FK_Device_ControlledVia != $PK_Device
"
ExternalStorageDevices=$(RunSQL "$Q")

for Device in $ExternalStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_Host=$(Field 2 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"
	Device_BlockDevice=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID and FK_DeviceData=$DD_BLOCK_DEVICE")

	Fstab_ExternalStorageDevices="$Fstab_ExternalStorageDevices\n$Device_ID -fstype=nfs,tcp,soft,timeo=1,retrans=1,retry=0 ${Device_Host}:${Device_MountPoint}"
	#stab_ExternalStorageDevices="$Fstab_ExternalStorageDevices\n${Device_Host}:${Device_MountPoint} $Device_MountPoint nfs rsize=8192,wsize=8192,intr,tcp 1 1"
done

PopulateSection "/etc/auto.PlutoStorageDevices" "ExternalStorageDevices" "$Fstab_ExternalStorageDevices"


TPL_BUFFALO_HDHG300LAN=1794
## Look for Buffalo NAS mounts
Q="
SELECT
	PK_Device,
	IPaddress
FROM
	Device
WHERE
	FK_DeviceTemplate=$TPL_BUFFALO_HDHG300LAN
"
BuffaloDevices=$(RunSQL "$Q")

for Device in $BuffaloDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_Host=$(Field 2 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"

	Automount_Buffalo="$Aautomount_Buffolo\n$Device_ID	-fstype=smbfs,guest	://${Device_Host}/share"
done

PopulateSection "/etc/auto.PlutoStorageDevices" "Buffalo HDHG300LAN" "$Automount_Buffalo"

## Reload automounter daemon
/etc/init.d/autofs reload
