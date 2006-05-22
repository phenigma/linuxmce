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


## Lookup for other computers storage devices and add them to automount as nfs shares
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

	Fstab_ExternalStorageDevices="$Fstab_ExternalStorageDevices\n$Device_ID -fstype=nfs,tcp,soft,timeo=5,retrans=1,retry=0 ${Device_Host}:${Device_MountPoint}"
	#stab_ExternalStorageDevices="$Fstab_ExternalStorageDevices\n${Device_Host}:${Device_MountPoint} $Device_MountPoint nfs rsize=8192,wsize=8192,intr,tcp 1 1"
done

PopulateSection "/etc/auto.PlutoStorageDevices" "ExternalStorageDevices" "$Fstab_ExternalStorageDevices"


## Look for Buffalo NAS mounts
TPL_BUFFALO_HDHG300LAN=1794
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

	Automount_Buffalo="$Automount_Buffalo\n$Device_ID	-fstype=smbfs,guest,dmask=777,fmask=777	://${Device_Host}/share"
done

PopulateSection "/etc/auto.PlutoStorageDevices" "Buffalo HDHG300LAN" "$Automount_Buffalo"

## Look for Maxtor NAS mounts
#TPL_MAXTOR_NAS=1770
#DD_SHARE_NAME=126
#Q="
#SELECT
#	PK_Device,
#	IPaddress
#FROM
#	Device
#WHERE
#	FK_DeviceTemplate=$TPL_MAXTOR_NAS
#"
#MaxtorDevices=$(RunSQL "$Q")
#
#for Device in $MaxtorDevices; do
#	Device_ID=$(Field 1 "$Device")
#	Device_Host=$(Field 2 "$Device")
#	Device_ShareName=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID and FK_DeviceData=$DD_SHARE_NAME")
#	
#	Automount_Maxtor="$Automount_Maxtor\n$Device_ID		-fstype=nfs,soft,timeo=1,retrans=1,retry=0,rsize=32768,wsize=32768 ${Device_Host}:${Device_ShareName}"
#done
#
#PopulateSection "/etc/auto.PlutoStorageDevices" "Maxtor 1770" "$Automount_Maxtor"

## Reload automounter daemon
/etc/init.d/autofs reload
