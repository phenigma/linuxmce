#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

RaidDevice=$1
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $BLOCK_DEVICE_ID"
array=$(RunSQL "$Q")

mdadm -S $array

Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $RaidDevice"
HardDriveList=$(RunSQL "$Q")
for Drive in $HardDriveList; do
	Q="SELECT Block.IK_DeviceData 
	   FROM Device 
	   JOIN Device_DeviceData Block ON Block.FK_Device = PK_Device AND Block.FK_DeviceData = $BLOCK_DEVICE_ID 
	   WHERE PK_Device=$Drive"
	R=$(RunSQL "$Q")
    	/usr/pluto/bin/delete_drive.sh '1' $array $R
done


rm -f $array
rm -f "$array:" "/dev/.static$array" "/dev/.tmp.$array"

