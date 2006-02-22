#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

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
		IK_DeviceData 
	FROM 
		Device_DeviceData
        WHERE 
		FK_Device=$PK_Device AND FK_DeviceData=XX?
"
R=$(RunSQL "$SQL")
IK_DeviceData=$(Filed 1 "$R")

dc1394_vloopback --daemon --video1394=/dev/video1394-$IK_DeviceData

