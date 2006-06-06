#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

## Only run on Core
if [[ -f /etc/diskless.conf ]]; then
	exit 0
fi

TPL_BUFFALO_HDHG300LAN=1794
TPL_GENERIC_INTERNAL_DRIVE=1790
TPL_MAXTOR_NAS=1770
DD_USE_PLUTO_DIR_STRUCTURE=130
DD_DIRECTORIES=153
DD_USERS=3

## Remove old symlinks from home
find /home/ -lname "*/mnt/device/*" -print0 | xargs -0 rm -f

## Lookup our internal storage devices in the db
Q="SELECT PK_Device, Description  FROM Device WHERE FK_DeviceTemplate IN ($TPL_GENERIC_INTERNAL_DRIVE, $TPL_BUFFALO_HDHG300LAN)"
InternalOwnStorageDevices=$(RunSQL "$Q")

for Device in $InternalOwnStorageDevices; do
	Device_ID=$(Field 1 "$Device")
	Device_Description=$(Field 2 "$Device")
	Device_MountPoint="/mnt/device/$Device_ID"
	
	Device_UsePlutoDirStructure=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_USE_PLUTO_DIR_STRUCTURE")
	
	Device_Directories=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_DIRECTORIES")
	Device_Directories=$(Field "1" "$Device_Directories")

	Device_Users=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_USERS")
	Device_Users=$(Field "1" "$Device_Users")

	echo -e "\n## Generating symlinks for storage device $Device_Description ($Device_ID)"
	
	## Sanitize Device_Directories/Users
	if [[ $Device_Directories == "" ]]; then
		Device_Directories="pictures,audio,documents,videos"
	fi

	if [[ $Device_Users == "" ]]; then
		Device_Users="0"
	fi

	Device_Directories=$(echo $Device_Directories | sed 's/ *, */,/g')
	Device_Directories=$(echo $Device_Directories | sed 's/^ *//g')
	Device_Directories=$(echo $Device_Directories | sed 's/ *$//g')

	Device_Users=$(echo $Device_Users | sed 's/ *, */,/g')
	Device_Users=$(echo $Device_Users | sed 's/^ *//g')
	Device_Users=$(echo $Device_Users | sed 's/ *$//g')
	
	
	## Create the symlinks
	countDirs=$(echo $Device_Directories | sed 's/,/\n/g' | wc -l)
	countUsers=$(echo $Device_Users | sed 's/,/\n/g' | wc -l)
	
	for i in `seq 1 $countDirs`; do
		mediaDir=$(echo $Device_Directories | cut -d',' -f$i)
		
		for j in `seq 1 $countUsers`; do
			user=$(echo $Device_Users | cut -d',' -f$j)
			
			if [[ $user == 0 ]]; then
				userDir="public"
			else
				userDir="user_$user"
			fi
			
			symlinkDestination="/home/$userDir/data/$mediaDir/$Device_Description [$Device_ID]"
			if [[ "$Device_UsePlutoDirStructure" == "1" ]]; then
				symlinkSource="/mnt/device/$Device_ID/$userDir/data/$mediaDir"
			else
				symlinkSource="/mnt/device/$Device_ID/"
			fi
			ln -s "$symlinkSource" "$symlinkDestination"
			echo "$symlinkDestination -> $symlinkSource"
		done
	done

done
