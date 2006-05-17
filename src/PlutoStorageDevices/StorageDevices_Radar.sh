#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

TPL_GENERIC_INTERNAL_DRIVE=1790
DD_BLOCK_DEVICE=152

function substractParts {
	local availPart=$1
	local unavailPart=$2
	local retPart=""

	for aPart in $availPart ;do
		available=1

		for uPart in $unavailPart ;do
			if [[ "$aPart" == "$uPart" ]]; then
				available=0
			fi
		done

		if [[ $available == 1 ]]; then
			retPart="$retPart $aPart"
		fi
	done

	echo $retPart
}

function Detect {
	## Available partitions
	availPart=$(cat /proc/partitions | awk '/(sd|hd).[0-9]/ { print $4 }')

	## Remove Blacklisted partitions (ignored)
	touch /usr/pluto/share/StorageDevices_Blacklist.txt
	blackPart=$(cat /usr/pluto/share/StorageDevices_Blacklist.txt)
	availPart=$(substractParts "$availPart" "$blackPart")

	## Remove Already configured partitions
	Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = $TPL_GENERIC_INTERNAL_DRIVE AND FK_Device_ControlledVia=$PK_Device"
	InternalOwnStorageDevices=$(RunSQL "$Q")

	confPart=""
	for Device in $InternalOwnStorageDevices; do
		Device_ID=$(Field 1 "$Device")
		Device_BlockDevice=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_BLOCK_DEVICE" | sed 's/\/dev\///g')
		confPars="$confPart $Device_BlockDevice"
	done
	availPart=$(substractParts "$availPart" "$confPart")

	## Remove Mounted partitions
	mountedPart=$(mount | awk '/dev\/(sd|hd).[0-9]/ {print $1}' | sed 's/\/dev\///g')
	availPart=$(substractParts "$availPart" "$mountedPart")

	## Remove Unmountable partitions (swap / extended)
	auxPart=""
	for part in $availPart ;do
		## If is extended partition
		if file -s /dev/$part | grep -q "extended partition table$"  ;then
			continue
		fi

		## If is swap partition
		if file -s /dev/$part | grep -q " Linux/i386 swap file"  ;then
			continue
		fi

		auxPart="$auxPart $part"
	done
	availPart=$auxPart

	## Display the new internal disk drive found wizard on available orbiters
	Q="
		SELECT
			PK_Device
		FROM
			Device
			JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
			JOIN DeviceCategory ON PK_DeviceCategory = FK_DeviceCategory
		WHERE
			PK_DeviceCategory IN (5,2,3)
	"
	OrbiterList=$(RunSQL "$Q")

	OrbiterIDList=""
	for Orbiter in $OrbiterList; do
		if [[ $OrbiterIDList != "" ]]; then
			OrbiterIDList="$OrbiterIDList,"
		fi

		Orbiter_ID=$(Field 1 "$Orbiter")
		OrbiterIDList="$OrbiterIDList""$Orbiter_ID"
	done

	/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 159 228 109 "$availPart" 156 $PK_Device
}

function Blacklist {
	local BlockDevice=$1
	echo $BlockDevice >> /usr/pluto/share/StorageDevices_Blacklist.txt
}

Params=("$@")
if [[ "$Params" == "" ]]; then
	Params="-d"	
fi

for ((i = 0; i < ${#Params[@]}; i++)); do
        case "${Params[$i]}" in
		-d|--detect)
			Detect
			exit 0
		;;
                -b|--blacklist) 
			((i++))
			BlockDevice="${Params[$i]}"
			Blacklist $BlockDevice
			exit 0
		;;
        esac
done
