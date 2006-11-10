#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null || exit 1
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1

TPL_BUFFALO=1794
TPL_FILESERVER=1837
TPL_WINDOWS_SHARE=1768
TPL_NFS_SHARE=1769
TPL_INTERNAL_DRIVE=1790

DD_SHARE_NAME=126
DD_USERNAME=127
DD_PASSWORD=128
DD_ONLINE=195
DD_BLOCK_DEV=152
DD_READONLY=194


function SetDeviceOnline() {
	local Device_ID=$1
	local OnlineValue=$2
	local Q=""
	
	Q="UPDATE Device_DeviceData SET IK_DeviceData = '$OnlineValue' WHERE FK_Device = '$Device_ID' AND FK_DeviceData= '$DD_ONLINE'"
	RunSQL "$Q"
	if [[ $OnlineValue == "0" ]] ;then
		umount -lf /mnt/device/$Device_ID 2>/dev/null 1>/dev/null
	fi
	
	Log "Setting $Device_ID Online flag to $OnlineValue"
}

function SetKidsOnline() {
	local Device_ID=$1
	local OnlineValue=$2
	local Q=""
	local R=""
	local Device_Child=""
	
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia = '$Device_ID'"
	R=$(RunSQL "$Q")
	for Device_Child in $R ;do
		Device_Child=$(Field 1 "$Device_Child")
		SetDeviceOnline "$Device_Child" "$OnlineValue"
	done
}

function Log() {
	local Message=$1
	
	echo "$(date -R) $Message"
}

while : ;do
	## Get a list with all the File Servers from the database
	Q="
		SELECT 
			Device.PK_Device, 
			Device.IPaddress,
			Username.IK_DeviceData,
			Password.IK_DeviceData
		FROM 
			Device
			LEFT JOIN Device_DeviceData Username ON Username.FK_Device = Device.PK_Device AND Username.FK_DeviceData = '$DD_USERNAME'
			LEFT JOIN Device_DeviceData Password ON Password.FK_Device = Device.PK_Device AND Password.FK_DeviceData = '$DD_PASSWORD'
		WHERE 
			Device.FK_DeviceTemplate IN ($TPL_BUFFALO, $TPL_FILESERVER)
			
	"
	FileServerList=$(RunSQL "$Q")

	## Test each file server
	for FileServer in $FileServerList ;do 
		Device_ID=$(Field 1 "$FileServer")
		Device_IP=$(Field 2 "$FileServer")
		Device_Username=$(Field 3 "$FileServer")
		Device_Password=$(Field 4 "$FileServer")
		
		if [[ "$Device_IP" == "" ]] ;then
			SetKidsOnline "$Device_ID" "0"
			Log "Device $Device_ID has no IP address associated with it."
			continue
		fi

		## Test to see if the file server is online or not
		ping -qnc 1 -W 1 "$Device_IP" &> /dev/null
		HostIsUp=$?
		if [[ "$HostIsUp" != "0" ]] ;then
			SetKidsOnline "$Device_ID" "0"
			Log "Device $Device_ID ($Device_IP) doesn't respond to our ping."
			continue
		fi

		
		Log "Device $Device_ID ($Device_IP) is online trying each of samba kids to see it they are still available"
		## Get a list of samba shares related to this device
		Q="
		SELECT 
			PK_Device,
			ShareName.IK_DeviceData,
			Username.IK_DeviceData,
			Password.IK_DeviceData
		FROM
			Device
			LEFT JOIN Device_DeviceData ShareName ON ShareName.FK_Device = Device.PK_Device AND ShareName.FK_DeviceData = '$DD_SHARE_NAME'
			LEFT JOIN Device_DeviceData Username ON Username.FK_Device = Device.PK_Device AND Username.FK_DeviceData = '$DD_USERNAME'
			LEFT JOIN Device_DeviceData Password ON Password.FK_Device = Device.PK_Device AND Password.FK_DeviceData = '$DD_PASSWORD'
		WHERE
			FK_Device_ControlledVia = '$Device_ID'
			AND
			FK_DeviceTemplate = '$TPL_WINDOWS_SHARE'
		"
		SambaShareList=$(RunSQL "$Q")

		## Test each samba share to see if is available or not
		for SambaShare in "$SambaShareList" ;do
			
			if [[ "$SambaShare" == "" ]] ;then
				continue
			fi
			
			Share_ID=$(Field "1" "$SambaShare")
			Share_Name=$(Field "2" "$SambaShare")
			Share_Username=$(Field "3" "$SambaShare")
			Share_Password=$(Field "4" "$SambaShare")

			## Test if the share is still in the list
			smbclient -U $Device_Username%$Device_Password --list=//$Device_IP --grepable 2>/dev/null | grep "^Disk" | cut -d'|' -f2 | grep -q "^${Share_Name}$"
			isShareInList=$?
			
			if [[ "$isShareInList" != "0" ]] ;then
				Log "Share $Share_ID ($Share_Name) is not advertised by the smb server"
				SetDeviceOnline "$Share_ID" "0"
				continue
			fi

			## Test if the share is still mountable with the username/password that we have
			smbclient -U $Share_Username%$Share_Password "//$Device_IP/$Share_Name" -c 'ls' 1>/dev/null 2>/dev/null
			isShareMountable=$?
			
			if [[ "$isShareMountable" != "0" ]] ;then
				Log "Share $Share_ID ($Share_Name) is not mountable with the username/pass that i have"
				SetDeviceOnline "$Share_ID" "0"
				continue
			fi

			SetDeviceOnline "$Share_ID" "1"
		done


		## Get a list of nfs shares related to this device
		Q="
                SELECT 
                        PK_Device,
                        ShareName.IK_DeviceData,
                        Username.IK_DeviceData,
                        Password.IK_DeviceData
                FROM
                        Device
                        LEFT JOIN Device_DeviceData ShareName ON ShareName.FK_Device = Device.PK_Device AND ShareName.FK_DeviceData = '$DD_SHARE_NAME'
                        LEFT JOIN Device_DeviceData Username ON Username.FK_Device = Device.PK_Device AND Username.FK_DeviceData = '$DD_USERNAME'
                        LEFT JOIN Device_DeviceData Password ON Password.FK_Device = Device.PK_Device AND Password.FK_DeviceData = '$DD_PASSWORD'
                WHERE
                        FK_Device_ControlledVia = '$Device_ID'
                        AND
                        FK_DeviceTemplate = '$TPL_NFS_SHARE'
                "
		NFSShareList=$(RunSQL "$Q")

		## Test each nfs share to see if is available on not
		for NFSShare in "$NFSShareList" ;do
			if [[ "$NFSShare" == "" ]] ;then 
				continue
			fi
			
			Share_ID=$(Field "1" "$NFSShare")
                        Share_Name=$(Field "2" "$NFSShare")
                        Share_Username=$(Field "3" "$NFSShare")
                        Share_Password=$(Field "4" "$NFSShare")
								
			## Test if the share is still in the list
			showmount -e $Device_IP | cut -d ' ' -f1 | grep "^${ShareName}$"
			isShareInList=$?

			if [[ "$isShareInList" != "0" ]] ;then
				Log "Share $Share_ID ($Share_Name) is not advertised by the nfs server"
				SetDeviceOnline "$Share_ID" "0"
				continue
			fi

			SetDeviceOnline "$Share_ID" "1"
		done
	done

	## Get a list of internal drives
	Q="
		SELECT
			PK_Device,
			FK_Device_ControlledVia,
			BlockDev.IK_DeviceData
		FROM
			Device
			LEFT JOIN Device_DeviceData BlockDev ON BlockDev.FK_Device = Device.PK_Device AND BlockDev.FK_DeviceData = '$DD_BLOCK_DEV'
		WHERE
			FK_DeviceTemplate = '$TPL_INTERNAL_DRIVE'
	"
	InternalDriveList=$(RunSQL "$Q")
	
	## Test each internal drive and see if is available
	for InternalDrive in "$InternalDriveList" ;do
		if [[ "$InternalDrive" == "" ]] ;then
			continue;
		fi

		IDrive_ID=$(Field "1" "$InternalDrive")
		IDrive_Parent=$(Field "2" "$InternalDrive")
		IDrive_BlockDev=$(Field "3" "$InternalDrive")

		## Is one of our internal drives
		if [[ "$IDrive_Parent" == "$PK_Device" ]] ;then
			## 

		## Is a internal drive located on a remote computer
		else
			echo "Not mine ($BlockDev)"
		fi
	done
	

	sleep 10
done
