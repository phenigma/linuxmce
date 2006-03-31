#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Maxtor_openmss_EnableNFS.sh

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

## Check Device ip address
if [[ "$Device_IP" == "" ]]; then
	Q="SELECT IPaddress FROM Device WHERE PK_Device = '$Device_ID'"
	Device_IP=$(RunSQL "$Q")
fi
if [[ "$Device_IP" == "" ]]; then
	echo "ERROR: No IP associated with the device $Device_ID"
fi

/usr/pluto/bin/StorageDevices_Mounts.sh
/usr/pluto/bin/StorageDevices_ExportsNFS.sh
/usr/pluto/bin/StorageDevices_Symlinks.sh
/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device_ID
