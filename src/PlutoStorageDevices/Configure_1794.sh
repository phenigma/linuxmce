#!/bin/bash
#
# Configuration script for : Device Template 1794 - Buffalo NAS HD-HG300LAN
#

. /usr/pluto/bin/SQL_Ops.sh

DD_USERNAME=127
DD_PASSWORD=128
DD_SHARE=126

pid=$$

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

## Create a temporary mount diretory where we can test the share mounts
tempMntDir=/tmp/mnt/$pid
mkdir -p $tempMntDir

## Get a list of samba shares exported by this device
for share in $(smbclient --no-pass --list=//$Device_IP  --grepable | grep "^Disk" | cut -d'|' -f2) ;do
	
	## Try to mount them without a password
	mount -o guest //$Device_IP/$share $tempMntDir
	success=$?
	
	## If the mount succeded withoud a password :
	if [[ "$success" == "0" ]] ;then

		## We umount the share from the temp dir
		umount -f -l $tempMntDir
		## And notify the router about our succes so he wouldn't ask the user for password
		/usr/pluto/bin/MessageSend dcerouter $Device_ID -1001 2 65 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share"

	## If the mound didn't succed without a user password
	else
		## We should try use our user and password on this share to see if it works
		## Suggestion : try brute-force the password so we won't need to ask the user, this would be super user frendly :))
		R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_USERNAME")
		Device_Username=$(Field "1" "$R")
		R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$Device_ID AND FK_DeviceData=$DD_PASSWORD")
		Device_Password=$(Field "1" "$R")

		mount -o username=${Device_User},password=${Device_Password} //$Device_IP/$share $tempMntDir
		success=$?

		
		if [[ $success ]] ;then
			umount -f -l $tempMntDir
			/usr/pluto/bin/MessageSend dcerouter $Device_ID -1001 2 65 52 3 53 2 49 1768 55 "182|0|$DD_SHARE|$share|$DD_USERNAME|$Device_Username|$DD_PASSWORD|$Device_Password"
		else
			/usr/pluto/bin/MessageSend dcerouter $Device_ID -1001 2 65 52 3 53 2 49 1768 55 "182|1|$DD_SHARE|$share"
		fi
	fi
done
rmdir $tempMntDir

#/usr/pluto/bin/StorageDevices_PlutoDirStructure.sh -d $Device_ID
#/usr/pluto/bin/StorageDevices_Setup.sh --cascade
