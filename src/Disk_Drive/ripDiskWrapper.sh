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
	echo "$0 <Disk_Drive device id> <Media_Plugin device id> <target file name> <source device> <disk type> <ownerID> ['<tracklist>']";
	echo "tracklist format: t1,name1[|t2,name2[|t3,name3]...]"
	echo "  t1 = track number"
	echo "  name1 = file name track is to be saved as"
}

[ "$#" -lt "6" ] && printUsage && exit;

diskDriveDeviceID=$1;
mediaPluginDeviceID=$2;
targetFileName=$3;
sourceDevice=$4;
diskType=$5;
ownerID=$6;
trackList="$7";

# Disk type possbile values:
#	0 DISCTYPE_CD_AUDIO 
#	1 DISCTYPE_CD
#   2 DISCTYPE_DVD_VIDEO 
#	6 DISCTYPE_CD_MIXED 
#	7 DISCTYPE_CD_VCD

echo "Ripping $sourceDevice to \"$targetFileName\" with a disk of type $diskType for $ownerID";

command="";
result=$ERR_NONE;
case $diskType in 
	2)
		command="nice -n 15 dd if=$sourceDevice of=\"/home/public/data/movies/$targetFileName.in-progress-dvd\" bs=10M"
	;;
	0|1|6|7|8)
		Dir="/home/public/data/music/$targetFileName"
		command="nice -n 15 cdparanoia -d $sourceDevice \$Track - | flac -o $Dir/\$FileName.in-progress-flac -"
	;;
	*)	result=$ERR_NOT_SUPPORTED_YET;;
esac

echo $command;
if [ "$command" == "" ]; then 
	/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceID -1001 2 35 20 $result 35 "$targetFileName";
	exit;
fi

if [[ "$diskType" == 2 ]]; then
	if eval $command; then
		/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceID -1001 2 35 20 $ERR_SUCCESS 35 "$targetFileName";
		mv -f "/home/public/data/movies/$targetFileName.in-progress-dvd" "/home/public/data/movies/$targetFileName.dvd";
	else
		/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceID -1001 2 35 20 $ERR_RESULT_FAILURE 35 "$targetFileName";
		rm "/home/public/data/movies/$targetFileName.in-progress-dvd";
	fi
elif [[ "$diskType" == 0 || "$diskType" == 1 || "$diskType" == 6 || "$diskType" == 7 || "$diskType" == 8 ]]; then
	mkdir -p "$Dir"
	for File in ${trackList//|/ }; do
		Track=${File%,*}
		FileName=${File#*,}
#		if ! nice -n 15 cdparanoia -d $sourceDevice $Track - | flac -o $Dir/$FileName.in-progress-flac -; then
		if ! eval $command; then
			/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceID -1001 2 35 20 $ERR_RESULT_FAILURE 35 "$FileName"
			exit
		fi
		mv $Dir/$FileName.in-progress-flac $Dir/$FileName.flac
	done
	/usr/pluto/bin/MessageSend dcerouter $diskDriveDeviceID -1001 2 35 20 $ERR_SUCCESS 35 "$targetFileName"
fi
