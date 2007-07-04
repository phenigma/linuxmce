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

	## Remove Mounted partitions
	mountedPart=$(mount | awk '/dev\/(sd|hd).[0-9]/ {print $1}' | sed 's/\/dev\///g')
	availPart=$(substractParts "$availPart" "$mountedPart")

	## Remove Unmountable partitions (swap / extended)
	auxPart=""
	for part in $availPart ;do
		## If is extended partition
		if file -s /dev/$part | grep -q "extended partition table"  ;then
			continue
		fi

		## If is swap partition
		if file -s /dev/$part | grep -q " Linux/i386 swap file"  ;then
                        if ! grep -q "^/dev/$part" /proc/swaps ;then
				echo "Enableing swap partition /dev/$part"      
				swapon /dev/$part
			fi

			continue
		fi

		auxPart="$auxPart $part"
	done
	availPart=$auxPart

	## Test if we found any available partitions
	if [[ $availPart != "" ]] ;then
		
		for partition in $availPart ;do
			#/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 159 228 109 "/dev/$partition" 156 $PK_Device 163 "$InfoMessage"

			## Get info about this partition
			halUDI=$(hal-find-by-property --key 'block.device' --string '/dev/$partition')
			
			Send="false"
			Count=0
			while [[ "$Sent" == "false" ]] ;do
				/usr/pluto/bin/MessageSend $DCERouter $PK_Device -1001 2 65 55 "152|/dev/$partition" 52 8 49 1790	
				err=$?

				if [[ "$err" == "0" ]] ;then
					Sent="true"
				else
					Count=$(( Count + 1 ))
					sleep 2
				fi


				if [[ "$Count" == "10" ]] ;then
					Send="true"
				fi
			done
		done
	fi
}

Detect
