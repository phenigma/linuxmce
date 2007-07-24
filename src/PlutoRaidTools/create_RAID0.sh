#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Device=$1
RAID0_DEVICE_TEMPLATE=1854
NEW_ADD_ID=204
HARD_DRIVE_DEVICE_TEMPLATE=1850
BLOCK_DEVICE_ID=152
SPARE_ID=202
DISK_SIZE_ID=201

if [[ "$3" != "demonized" ]] ;then
	
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

			Q="
				SELECT 
					Block.IK_DeviceData, 
					Spare.IK_DeviceData 
				FROM Device 
					JOIN Device_DeviceData Block ON Block.FK_Device = PK_Device AND Block.FK_DeviceData = $BLOCK_DEVICE_ID
					JOIN Device_DeviceData Spare ON Spare.FK_Device = PK_Device AND Spare.FK_DeviceData = $SPARE_ID
				WHERE 
					PK_Device = $Drive;
			"
			R=$(RunSQL "$Q")
			Disk=$(Field 1 "$R")
			IsSpare=$(Field 2 "$R")
			ActiveDrives="$ActiveDrives "$Disk
			NrDrives=$(($NrDrives+1))
		done
		#NrDrives=$(($NrDrives+1))
		#create the array with mdadm
		rm -f "/dev/.static$name"
		echo "y" | mdadm --create $name --auto=yes --level=0 --raid-devices=$NrDrives $ActiveDrives

		sleep 3
		raidSize=$(mdadm --query $name | head -1 |cut -d' ' -f2)
		Q="UPDATE Device_DeviceData SET IK_DeviceData = '$raidSize' WHERE FK_Device = $Device and FK_DeviceData = $DISK_SIZE_ID"
		RunSQL "$Q"
		#/usr/pluto/bin/start_RAID_monitoring.sh $name
		screen -S FormatingRaid0 -d -m /bin/bash $0 $Device $name demonized
		exit 0
	fi
else
	## Format the new raid
	LogFile="/usr/pluto/var/${Device}_Raid.log"
	echo "FORMAT,1" > $LogFile

	echo "y" | mkfs.ext3 $2 > /var/log/mkfs_${Device} &

	percent=0
	while "$percent" != 100 ;do
		sleep 4
		mkfsOutput=$(cat /var/log/mkfs_${Device}  | grep "Writing inode tables: " | sed -r 's/.*[^0-9]([0-9]+\/[0-9]+|done)[^0-9]*/\1/')a

		if [[ "$mkfsOutput" == "" ]] ;then
			percent=1

		elif [[ "$mkfsOutput" == "done" ]] ;then
			percent=100

		else
			done=$(echo $mkfsOutput | cut -d'/' -f1)
			total=$(echo $mkfsOutput | cut -d'/' -f2)
			percent=$(((done * 100) / total ))
		fi

		echo "FORMAT,$percent" > $LogFile
	done


	## Create a pluto directory structure (depending on UsePlutoDirStructure device data)
	/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device
	/usr/pluto/bin/StorageDevices_Setup.sh
	
	rm $LogFile
fi
