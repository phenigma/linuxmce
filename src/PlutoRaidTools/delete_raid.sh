#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

RaidDevice=$1
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = $RaidDevice and FK_DeviceData = $BLOCK_DEVICE_ID"
array=$(RunSQL "$Q")

touch /tmp/info.raid
mdadm --detail $array >/tmp/info.raid

while read line; do
    drive=$(echo ${line}|cut -d' ' -f6)
    check=$(echo $drive | grep dev)
    if [[ -z $check ]]; then
	drive=$(echo ${line}|cut -d' ' -f7)	    
    fi
    if [[ $drive != "/dev/$array:" ]]; then
    	/usr/pluto/bin/delete_drive.sh '1' $array $drive
    fi
done < <(cat /tmp/info.raid | grep /dev)

mdadm -S $array
rm -f $array "$array:"
rm -f /tmp/info.raid
