#!/bin/bash


# This is stupid i know but it is all that i could find as a solution in the hurry
#	See Disk_Drive.h for the treated values.
ERR_NOT_SUPPORTED_YET=6
ERR_RESULT_FAILURE=6
ERR_SUCCESS=5


# internal sript errors
ERR_NONE=0

function printUsage
{
	echo "$0 <Disk_Drive device id> <Media_Plugin device id> <target file name> <source device> <disk type> <ownerID>";
}

[ "$#" -ne "6" ] && printUsage && exit;

diskDriveDeviceID=$1;
mediaPluginDeviceID=$2;
targetFileName=$3;
sourceDevice=$4;
diskType=$5;
ownerID=$6;

# Disk type possbile values:
#	0 DISCTYPE_CD_AUDIO 
#   2 DISCTYPE_DVD_VIDEO 
#	6 DISCTYPE_CD_MIXED 
#	7 DISCTYPE_CD_VCD

echo "Ripping $sourceDevice to \"$targetFileName\" with a disk of type $diskType for $ownerID";

command="";
result=$ERR_NONE;
case $diskType in 
	2)	command="nice -n 15 dd if=$sourceDevice of=\"/home/public/data/movies/$targetFileName.dvd\" bs=10M";;
	*)	result=$ERR_NOT_SUPPORTED_YET;;
esac

echo $command;
if [ "$command" == "" ]; then 
	/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceId $mediaPluginDeviceID 2 35 20 $result 35 "$targetFileName";
	exit;
fi

if eval $command; then 
	/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceId $mediaPluginDeviceID 2 35 20 $ERR_SUCCESS 35 "$targetFileName";
	mv -f "/home/public/data/movies/$targetFileName.dvd-in-progress" "/home/public/data/movies/$targetFileName.dvd";
else
	/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceId $mediaPluginDeviceID 2 35 20 $ERR_RESULT_FAILURE 35 "$targetFileName";
	rm "/home/public/data/movies/$targetFileName.dvd-in-progress";
fi
