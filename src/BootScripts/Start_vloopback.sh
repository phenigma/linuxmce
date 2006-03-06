#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

DEVICEDATA_PortChannel_Number=12
DEVICEDATA_Device=1

PipeDevOffset=30
PipeCount=16

function showUsage {
        echo "Usage: $0 -r <RouterID> -d <DeviceID>"
        echo
}

while getopts r:d: name
do
	echo "-"$name
        case $name in
                r) RouterID=$OPTARG ;;
                d) PK_Device=$OPTARG ;;
                ?)
                        showUsage
                        exit 1
                ;;
        esac
done

shift $(($OPTIND - 1))

SQL="
	SELECT
		FK_DeviceData,
		IK_DeviceData
	FROM
		Device_DeviceData
	WHERE
		FK_Device=$PK_Device
		AND
		FK_DeviceData IN ($DEVICEDATA_PortChannel_Number, $DEVICEDATA_Device)
"
R=$(RunSQL "$SQL")

for DeviceData in $R; do
	FK_DeviceData=$(Field 1 "$DeviceData")
	IK_DeviceData=$(Field 2 "$DeviceData")
	case "$FK_DeviceData" in
		"$DEVICEDATA_Device") Dev_IEEE1394="$IK_DeviceData" ;;
		"$DEVICEDATA_PortChannel_Number") Dev_Pipe="$IK_DeviceData" ;;
	esac
done

Dev_V4L=$((Dev_Pipe * 2 + PipeDevOffset))

modprobe video1394
modprobe raw1394
modprobe vloopback pipes=$PipeCount dev_offset=$PipeDevOffset
dc1394_vloopback "--video1394=/dev/video1394-$Dev_IEEE1394" "--vloopback=/dev/video$Dev_V4L" --pipe
