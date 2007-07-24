#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

event=$1
md=$2
device=$3
BLOCK_DEVICE_ID=152
NEW_ADD_ID=204
STATE_ID=200
DISK_SIZE_ID=201
SPARE_ID=202
HARD_DRIVE_DEVICE_TEMPLATE=1850
RAID5_DEVICE_TEMPLATE=1849
RAID1_DEVICE_TEMPLATE=1851
RAID0_DEVICE_TEMPLATE=1854

date=$(date)

if [[ -z $device ]]; then
	echo "($date) Catched event $event for $md" >> /var/log/pluto/RAID_monitoring.log
else	
	echo "($date) Catched event $event for $device from $md" >> /var/log/pluto/RAID_monitoring.log
fi

case "$event" in
	#"DeviceDisappeared" )
	 #Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
	 #DeviceID=$(RunSQL "$Q")
	 #Q="UPDATE Device_DeviceData SET IK_DeviceData = 3 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
	 #$(RunSQL "$Q")
	 #Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Device appears to no longer be configured' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 #$(RunSQL "$Q")
	#;;
	"RebuildStarted" )
	 Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
     DeviceID=$(RunSQL "$Q")
     Q="UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
     RunSQL "$Q"
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Building Started' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 RunSQL "$Q"
	raidSize=$(mdadm --query $md | head -1 |cut -d' ' -f2)
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$raidSize' WHERE FK_Device = $DeviceID and FK_DeviceData = $DISK_SIZE_ID"
	RunSQL "$Q"
	 ;; 
	"RebuildFinished" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 4 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
	RunSQL "$Q"
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Done' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID" 
	RunSQL "$Q"
	raidSize=$(mdadm --query $md | head -1 |cut -d' ' -f2)
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$raidSize' WHERE FK_Device = $DeviceID and FK_DeviceData = $DISK_SIZE_ID"
	RunSQL "$Q"
	;;
	"Rebuild"* )
	 Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
     DeviceID=$(RunSQL "$Q")
     Q="UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
     RunSQL "$Q"
	 len=$((${#event}))
	 progress=${event:$len-2:$len}
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Building in progress ($progress %)' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 RunSQL "$Q"
	raidSize=$(mdadm --query $md | head -1 |cut -d' ' -f2)
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$raidSize' WHERE FK_Device = $DeviceID and FK_DeviceData = $DISK_SIZE_ID"
	RunSQL "$Q"
	 ;;
	"DegradedArray" )
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
		DeviceID=$(RunSQL "$Q")

		Q="UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
		RunSQL "$Q"
	 
		Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Devices is workable but degraded for now. Will start rebuilding ' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
		RunSQL "$Q"


	"Fail" )
	failedDevs=$(mdadm --detail $md | awk '/faulty/ {print $6}')
	for failedDev in $failedDevs; do
		Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$failedDev' AND FK_DeviceData = $BLOCK_DEVICE_ID"
		DeviceID=$(RunSQL "$Q")
		Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Failed disk' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
		RunSQL "$Q"
	done
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="SELECT COUNT(PK_Device) FROM Device WHERE FK_DeviceTemplate = $HARD_DRIVE_DEVICE_TEMPLATE AND FK_Device_ControlledVia = $DeviceID"
	HardDriveNr=$(RunSQL "$Q")
	Q="SELECT COUNT(IK_DeviceData)
	   FROM Device_DeviceData 
	   INNER JOIN Device ON Device_DeviceData.FK_Device = Device.PK_Device 
	   WHERE Device.FK_Device_ControlledVia = $DeviceID AND
	         Device_DeviceData.FK_DeviceData = $STATE_ID AND 
		 Device_DeviceData.IK_DeviceData = 'Failed disk'"
        FailedDriveNr=$(RunSQL "$Q")
	Q="SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = $DeviceID"
	RaidTemplate=$(RunSQL "$Q")
	case "$RaidTemplate" in 
		"$RAID5_DEVICE_TEMPLATE" | "$RAID1_DEVICE_TEMPLATE" )
		if (( $HardDriveNr == 2 && $FailedDriveNr == 1 )) ||  (( $FailedDriveNr > 1 )) ;then
			Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Damaged. Device appears to no longer be configured ' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
			RunSQL "$Q"
		else	
			Q="UPDATE Device_DeviceData SET IK_DeviceData = 'One active disk is down. In order to correct the problem please add a spare disk. If you already have a spare disk attached, rebuilding will automatically start!' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
			RunSQL "$Q"
		fi
		;;
		"$RAID0_DEVICE_TEMPLATE" )
		if (( $FailedDriveNr >= 1 )) ;then
			Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Damaged. Device appears to no longer be configured ' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
			RunSQL "$Q"
		fi
		;;
	esac
	;;
	"FailSpare" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$device' AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Spare failed to rebuild a faulty disk' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	RunSQL "$Q"
	;;
	"SpareActive" )
	 Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$device' AND FK_DeviceData = $BLOCK_DEVICE_ID"
	 DeviceID=$(RunSQL "$Q")
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = 'OK' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 RunSQL "$Q"
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = '0' WHERE FK_Device = $DeviceID and FK_DeviceData = $SPARE_ID"
	 RunSQL "$Q"
	;;
	"DegradedArray" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 5 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
	RunSQL "$Q"
	Q="SELECT COUNT(IK_DeviceData)
	   FROM Device_DeviceData 
	   INNER JOIN Device ON Device_DeviceData.FK_Device = Device.PK_Device 
	   WHERE Device.FK_Device_ControlledVia = $DeviceID AND
	         Device_DeviceData.FK_DeviceData = $SPARE_ID AND 
		 Device_DeviceData.IK_DeviceData = 1"
        SpareNr=$(RunSQL "$Q")
	if (( $SpareNr >= 1 )) ;then
		Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Start Building' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID" 
		RunSQL "$Q"
	else
		Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Degraded device. Add a spare disk to correct the problem!' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID" 
		RunSQL "$Q"
	fi
	;;
esac
	
