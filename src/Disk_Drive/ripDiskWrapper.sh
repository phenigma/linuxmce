#!/bin/bash


. /usr/pluto/bin/Config_Ops.sh

# This is stupid i know but it is all that i could find as a solution in the hurry
#	See Disk_Drive.h for the treated values.
ERR_NOT_SUPPORTED_YET=6
ERR_RESULT_FAILURE=6
ERR_SUCCESS=5


# internal sript errors
ERR_NONE=0

function isFullDisk
{
	file="/${1%/*}"
	blocks=$(stat --format=%a -f "$file")
	block_size=$(stat --format=%S -f "$file")
	size=$((blocks * block_size / 1024 / 1024))
	if [[ "$size" -gt 1 ]]; then
		echo '0'
	else
		echo '1'
	fi
}

function isReadOnly
{
	file="$1"
	path=${file%/*}
	touch "$path"/testing 2>/dev/null
	if [[ -e "$path"/testing ]]; then
		rm "$path"/testing
		echo '0'
	else
		echo '1'
	fi
}


function printUsage
{
	echo "$0 <Disk_Drive device id> <job id> <task id> <target file name> <source device> <disk type> <ownerID> <rip format> ['<tracklist>']";
	echo "tracklist format: t1,name1[|t2,name2[|t3,name3]...]"
	echo "  t1 = track number"
	echo "  name1 = file name track is to be saved as"
}

[[ "$#" -lt "6" ]] && printUsage && exit;

for i in "$@"; do
	echo "Parameter: $i"
done

diskDriveDeviceID="$1"
jobID="$2"
taskID="$3"
targetFileName="$4"
sourceDevice="$5"
diskType="$6"
ownerID="$7"
ripFormatString="$8" # mp3, ogg, flac, wav
trackList="$9"

ripFormat=${ripFormatString%%;*}

# Disk type possbile values:
#	0 DISCTYPE_CD_AUDIO 
#	1 DISCTYPE_CD
#   2 DISCTYPE_DVD_VIDEO 
#	6 DISCTYPE_CD_MIXED 
#	7 DISCTYPE_CD_VCD

echo "Ripping $sourceDevice to \"$targetFileName\" with a disk of type $diskType for user $ownerID";

command="";
result=$ERR_NONE;
case $diskType in 
	2)
		ProgressOutput='>(/usr/pluto/bin/DiskCopy_ProgressExtract.sh|/usr/pluto/bin/Pluto_Progress.sh "$diskDriveDeviceID" "$jobID" "$taskID")'
		command='/usr/pluto/bin/disc_unlock "$sourceDevice"; nice -n 15 /usr/pluto/bin/disk_copy "$sourceDevice" "$targetFileName.dvd.in-progress" > '"$ProgressOutput"
	;;
	0|1|6|7|8)
		Dir="$targetFileName"
		case "$ripFormat" in
			wav)
				FinalExt="wav"
				OutputFile='"$Dir/$FileName.'"$FinalExt"'.in-progress"' # bare file
			;;
			
			ogg)
				FinalExt="ogg"
				OutputFile='>(oggenc -Q -o "$Dir/$FileName.'"$FinalExt"'.in-progress" -)' # encoder
			;;
			
			flac)
				FinalExt="flac"
				OutputFile='>(flac -o "$Dir/$FileName.'"$FinalExt"'.in-progress" -)' # encoder
			;;
			
			mp3)
				FinalExt="mp3"
				OutputFile='>(lame -h - "$Dir/$FileName.'"$FinalExt"'.in-progress")' # encoder
			;;
			
			*)
				echo "Unknown method: $ripFormat"
				exit 1
			;;
		esac
		ProgressOutput='>(/usr/pluto/bin/Paranoia_Progress.sh|/usr/pluto/bin/Pluto_Progress.sh "$diskDriveDeviceID" "$jobID" "$taskID" "$Dir/$FileName")'
		command='nice -n 15 cdparanoia -e -d "$sourceDevice" "$Track" - 2> '"$ProgressOutput"' > '"$OutputFile"
	;;
	*)	result=$ERR_NOT_SUPPORTED_YET;;
esac

echo "Command: $command"
if [[ "$command" == "" ]]; then 
	exit 1;
fi

trap "/usr/pluto/bin/cdop '$sourceDevice' unlock" EXIT # unlock tray on exit
/usr/pluto/bin/cdop "$sourceDevice" lock # lock tray

if [[ "$diskType" == 2 ]]; then
	if eval "$command"; then
		echo "Ripping successful"
		touch "$targetFileName.dvd.lock"
		mv -f "$targetFileName.dvd"{.in-progress,}
		exit 0;
	else
		if [[ "$(isFullDisk "$targetFileName")" == 1 ]]; then
			Message="Disk Full!"
		else
			if [[ "$(isReadOnly "$targetFileName")" == 1 ]]; then
				Message="Disk Read Only!"
			else		
				Message=$(</tmp/rip_message)
			fi
		fi
		echo "Ripping failed: $Message" >> /tmp/riplog
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
		rm "$targetFileName.dvd.in-progress";
		exit 1;
	fi
elif [[ "$diskType" == 0 || "$diskType" == 1 || "$diskType" == 6 || "$diskType" == 7 || "$diskType" == 8 ]]; then
	mkdir -p "$Dir"
	if [[ ! -d "$Dir" ]]; then
		$Message="Couldn't create directory";
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
		echo "Ripping failed: $Message" >> /tmp/riplog
		exit 1
	fi
	trackList=${trackList// /@~#}
	for File in ${trackList//|/ }; do
		File=${File//@~#/ }
		Track=${File%%,*}
		FileName=${File#*,}
		FileName=${FileName//\//-}
		
		echo "Track: $Track; Filename: $FileName"
		if /usr/pluto/bin/IsDataTrack "$sourceDevice" "$Track"; then
			echo "Skipping track '$Track' because it is a data track on a audio CD"
			continue
		fi
		
		if [[ -z "$FileName" ]]; then
			echo "Skipping track '$Track'. Empty filename"
			continue
		fi

		displaycmd="$command"
		displaycmd="${displaycmd//\\\"/\\\"}"
		echo "Executing: $(eval echo "\"$displaycmd\"")"
		if ! eval "$command"; then
			if [[ "$(isFullDisk "$targetFileName")" == 1 ]]; then
				Message="Disk Full!"
			else
				if [[ "$(isReadOnly "$targetFileName")" == 1 ]]; then
					Message="Disk Read Only!"
				else		
					Message=$(</tmp/rip_message)
				fi
			fi
			
			echo "Ripping failed: $Message" >> /tmp/riplog
			/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
			rm "$Dir/$Filename.$FinalExt.in-progress" &>/dev/null
			exit 1;
		fi
		echo "File ripped ok; moving: $Dir/$FileName.$FinalExt.in-progress"
		touch "$Dir/$FileName.$FinalExt.lock"
		mv "$Dir/$FileName.$FinalExt"{.in-progress,}
	done
	echo "Ripping successful"
	exit 0;
fi

echo "Exiting ripping script"

