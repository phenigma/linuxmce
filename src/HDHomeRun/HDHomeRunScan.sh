#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT IPAddress
        FROM Device
        WHERE PK_Device='$2'"
DeviceID=$(RunSQL "$Q")

Q="SELECT IK_DeviceData
	FROM Device_DeviceData
	WHERE FK_Device='$1' AND FK_DeviceData=12 LIMIT 1"
Port=$(RunSQL "$Q")

echo "$(date -R) HDHomeRunScan waiting for lock"
WaitLock "HDHomeRun_Setup" "HDHomeRun_Setup" nolog
echo "$(date -R) HDHomeRunScan obtained lock Starting /usr/pluto/bin/hdhomerun_mythconfig $1 $DeviceID $Port"

/usr/pluto/bin/hdhomerun_mythconfig $1 $DeviceID $Port $MySqlHost root "" mythconverg

if [[ "$?" == "0" ]] ;then
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 $1 20 1
else
	/usr/pluto/bin/MessageSend dcerouter -targetType template 0 36 2 72 26 $1 20 -1
fi

Unlock "HDHomeRun_Setup" "HDHomeRun_Setup" nolog
echo "$(date -R) HDHomeRunScan unlocked"
