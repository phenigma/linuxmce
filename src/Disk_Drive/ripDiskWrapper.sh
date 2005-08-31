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

for i in "$@"; do
	echo "Parameter: $i"
done

diskDriveDeviceID=$1
mediaPluginDeviceID=$2
targetFileName=$3
sourceDevice=$4
diskType=$5
ownerID=$6
ripFormat=$7 # mp3, ogg, flac, wav
trackList="$8"

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
		command="nice -n 15 /usr/pluto/bin/disk_copy $sourceDevice \"$targetFileName.in-progress-dvd\" > >(/usr/pluto/bin/DiskCopy_ProgressExtract.sh|/usr/pluto/bin/Pluto_Progress.sh $diskDriveDeviceID "$targetFileName.in-progress-dvd")"
	;;
	0|1|6|7|8)
		Dir="$targetFileName"
		command="nice -n 15 cdparanoia -e -d $sourceDevice \$Track - 2> >(/usr/pluto/bin/Paranoia_Progress.sh|/usr/pluto/bin/Pluto_Progress.sh $diskDriveDeviceID \"$Dir/\$FileName.in-progress-flac\") | flac -o \"$Dir/\$FileName.in-progress-flac\" -"
	;;
	*)	result=$ERR_NOT_SUPPORTED_YET;;
esac

echo "$command"
if [ "$command" == "" ]; then 
	exit 1;
fi

if [[ "$diskType" == 2 ]]; then
	if eval $command; then
		echo "Ripping successful"
		mv -f "$targetFileName.in-progress-dvd" "$targetFileName.dvd";
		exit 0;
	else
		echo "Ripping failed"
		rm "$targetFileName.in-progress-dvd";
		exit 1;
	fi
elif [[ "$diskType" == 0 || "$diskType" == 1 || "$diskType" == 6 || "$diskType" == 7 || "$diskType" == 8 ]]; then
	mkdir -p "$Dir"
	trackList=${trackList// /@~#}
	for File in ${trackList//|/ }; do
		File=${File//@~#/ }
		Track=${File%,*}
		FileName=${File#*,}
		
		echo "Track: $Track; Filename: $FileName"
		displaycmd="$command"
		displaycmd="${displaycmd//\\\"/\\\"}"
		echo "Executing: $(eval echo "\"$displaycmd\"")"
		if ! eval "$command"; then
			echo "Ripping failed"
			exit 1;
		fi
		echo "First file ripped ok moving: $Dir/$FileName.in-progress-flac"
		mv "$Dir/$FileName.in-progress-flac" "$Dir/$FileName.flac"
	done
	echo "Ripping successful"
	exit 0;
fi

echo "Exiting ripping script"

