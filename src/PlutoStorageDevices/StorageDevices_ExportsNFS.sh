#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh

TPL_GENERIC_INTERNAL_DRIVE=1790
DD_BLOCK_DEVICE=152


## Set some variables needed for exporting nfs
if [[ -f /etc/diskless.conf ]] ;then
        INTERNAL_ADDRESS="$ExtIP"
        INTERNAL_SUBNET_MASK="$ExtNetmask"
else
        INTERNAL_SUBNET_MASK="$IntNetmask"
        INTERNAL_ADDRESS="$IntIP"
fi

for i in 1 2 3 4; do
	IPDigit=$(echo $INTERNAL_ADDRESS | cut -d. -f$i)
	MaskDigit=$(echo $INTERNAL_SUBNET_MASK | cut -d. -f$i)
	NetDigit=$(($IPDigit & $MaskDigit))
	INTERNAL_SUBNET="$INTERNAL_SUBNET$Dot$NetDigit" && Dot="."
done
				

## Lookup our internal storage devices in the db
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $TPL_GENERIC_INTERNAL_DRIVE AND FK_Device_ControlledVia=$PK_Device"
InternalOwnStorageDevices=$(RunSQL "$Q")

for Device in $InternalOwnStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"
	Device_BlockDevice=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID and FK_DeviceData=$DD_BLOCK_DEVICE")

	Exports_InternalStorageDevices="$Exports_InternalStorageDevices\n$Device_MountPoint ${INTERNAL_SUBNET}/${INTERNAL_SUBNET_MASK}(rw,no_root_squash,no_all_squash,sync,nohide)"
done

PopulateSection "/etc/exports" "InternalStorageDevices" "$Exports_InternalStorageDevices"

## Check and start/reload the nfs-kernel-server
if [[ "$(pidof rpc.mountd)" == "" ]] ;then
	/etc/init.d/nfs-kernel-server restart
fi
/etc/init.d/nfs-kernel-server reload
