#!/bin/bash

#
# Configuration script for : Device Template 1790 - Generic Internal Drive
#

. /usr/pluto/bin/SQL_Ops.sh

DD_UUID=267

Params=("$@")
for ((i = 0; i < ${#Params[@]}; i++)); do
	case "${Params[$i]}" in
		-d) ((i++)); Device_ID="${Params[$i]}" ;;
		-i) ((i++)); Device_IP="${Params[$i]}" ;;
		-m) ((i++)); Device_MAC="${Params[$i]}" ;;
	esac
done

# Disable automatic filesystem checking for ext3 drives
Device_UUID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_UUID")
tune2fs -c 0 "/dev/disk/by-uuid/$Device_UUID" 2>/dev/null || :

/usr/pluto/bin/StorageDevices_Setup.sh --cascade

