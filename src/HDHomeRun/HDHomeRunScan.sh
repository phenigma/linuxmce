#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT IK_DeviceData
	FROM Device_DeviceData
	WHERE FK_Device='$1' AND FK_DeviceData=152 LIMIT 1"
DeviceID=$(RunSQL "$Q")

Q="SELECT IK_DeviceData
	FROM Device_DeviceData
	WHERE FK_Device='$1' AND FK_DeviceData=12 LIMIT 1"
Port=$(RunSQL "$Q")


/usr/pluto/bin/hdhomerun_mythconfig $1 $DeviceID $Port dcerouter root "" mythconverg

if [[ "$?" == "0" ]] ;then
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 $1 20 1
else
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 $1 20 -1
fi