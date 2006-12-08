#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Device=$1
RAID0_DEVICE_TEMPLATE=1854
NEW_ADD_ID=204
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152
SPARE_ID=202
DISK_SIZE_ID=201

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Device and FK_DeviceData = $NEW_ADD_ID"
NewAdd=$(RunSQL "$Q")
name=$(/usr/pluto/bin/get_RAID_name.sh)
name="/dev/$name"
Q="UPDATE Device_DeviceData SET IK_DeviceData = '$name' WHERE FK_Device = $Device and FK_DeviceData = $BLOCK_DEVICE_ID"
$(RunSQL "$Q")
#if is new added get the list of active drives and spares
if [[ $NewAdd == 0 ]] ;then
	Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $Device"
	HardDriveList=$(RunSQL "$Q")
	ActiveDrives=
	NrDrives=
	for Drive in $HardDriveList; do
		Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $Drive AND FK_DeviceData = $BLOCK_DEVICE_ID"
		R=$(RunSQL "$Q")
		Disk=$(echo $R | cut -d' ' -f1)
		#echo ">>$Disk<<"
		#echo ">>$IsSpare<<"
		ActiveDrives="$ActiveDrives "$Disk
		NrDrives=$(($NrDrives+1))
	done
	#NrDrives=$(($NrDrives+1))
	#create the array with mdadm
	echo "y" | mdadm --create $name --auto --level=0 --raid-devices=$NrDrives $ActiveDrives

	raidSize=$(mdadm --query $name | head -1 |cut -d' ' -f2)
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$raidSize' WHERE FK_Device = $Device and FK_DeviceData = $DISK_SIZE_ID"
	$(RunSQL "$Q")
	/usr/pluto/bin/start_RAID_monitoring.sh $name
fi
