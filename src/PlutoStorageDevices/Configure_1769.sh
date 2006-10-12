#!/bin/bash
#
# Configuration script for : Device Template 1769 - Generic NFS Share
#

. /usr/pluto/bin/SQL_Ops.sh

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

## Check PK_Device
if [[ "$Device_ID" == "" ]]; then
	echo "ERROR: No PK_Device assigned to this device"
fi

/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device_ID
/usr/pluto/bin/StorageDevices_Setup.sh --cascade
