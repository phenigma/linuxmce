#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

event=$1
md=$2
device=$3
BLOCK_DEVICE_ID=152
NEW_ADD_ID=204
STATE_ID=200
SPARE_ID=202

date=$(date)

if [[ -z $device ]]; then
	echo "($date) Catched event $event for $md" >> /tmp/raid_monitoring
else	
	echo "($date) Catched event $event for $device from $md" >> /tmp/raid_monitoring
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
     $(RunSQL "$Q")
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Building Started' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 $(RunSQL "$Q")
	 ;; 
	"RebuildFinished" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 4 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
	$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Done' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID" 
	$(RunSQL "$Q")
	;;
	"Rebuild"* )
	 Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
     DeviceID=$(RunSQL "$Q")
     Q="UPDATE Device_DeviceData SET IK_DeviceData = 1 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
     $(RunSQL "$Q")
	 len=$((${#event}))
	 progress=${event:$len-2:$len}
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Building in progress ($progress %)' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 $(RunSQL "$Q")
	 ;;
	"Fail" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$device' AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Failed disk' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	$(RunSQL "$Q")
	;;
	"FailSpare" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$device' AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Spare failed to rebuild a faulty disk' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	$(RunSQL "$Q")
	;;
	"SpareActive" )
	 Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$device' AND FK_DeviceData = $BLOCK_DEVICE_ID"
	 DeviceID=$(RunSQL "$Q")
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = 'OK' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID"
	 $(RunSQL "$Q")			 
	 Q="UPDATE Device_DeviceData SET IK_DeviceData = '0' WHERE FK_Device = $DeviceID and FK_DeviceData = $SPARE_ID"
	 $(RunSQL "$Q")
	;;
	"DegradedArray" )
	Q="SELECT FK_Device FROM Device_DeviceData WHERE IK_DeviceData = '$md'  AND FK_DeviceData = $BLOCK_DEVICE_ID"
	DeviceID=$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 5 WHERE FK_Device = $DeviceID and FK_DeviceData = $NEW_ADD_ID"
	$(RunSQL "$Q")
	Q="UPDATE Device_DeviceData SET IK_DeviceData = 'Start Building' WHERE FK_Device = $DeviceID and FK_DeviceData = $STATE_ID" 
	$(RunSQL "$Q")
	;;
esac
	
