#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

function archiveSyncMDfromCORE {
	local MD_Root=$1
	
	chroot $MD_Root apt-get autoclean
	cp -ru /var/cache/apt/archives $MD_Root/var/cache/apt/archives
}

function archiveSyncCOREfromMD {
	local MD_Root=$1

	cp -ru $MD_Root/var/cache/apt/archives /var/cache/apt/archives
}


## This directory shoul be there, but just in case
if [[ ! -d /usr/pluto/var/Updates ]]; then
	mkdir -p /usr/pluto/var/Updates
fi

## Start Logging this
mv /var/log/pluto/DownloadPackages.newlog /var/log/pluto/DownloadPackages.log
touch /var/log/pluto/DownloadPacakges.newlog
exec 6>&1
exec > /var/log/pluto/DownloadPacakges.newlog
echo "##  Updates Download Started at $(date -R)"

## Make sure we don't run more than once
UpdatesDownloadingStamp=$(date +%s)
if [[ -f /usr/pluto/var/Updates/DownloadUpdates.lock ]]; then
	exit 0;
else
	echo $UpdatesDownloadingStamp > /usr/pluto/var/Updates/DownloadUpdates.lock
fi
							
## Apt-get update for core
echo "##  Apt-get update for core"
apt-get update 1>/dev/null

## Get a list of all MDs
Q="
SELECT 
	PK_Device,
	IPaddress
FROM 
	Device
	JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE 
	PK_DeviceTemplate=28 
	AND
	FK_Device_ControlledVia IS NULL
"
MD_List=$(RunSQL "$Q")
MD_FsPath="/usr/pluto/diskless"
MD_Index=0

## Run apt-get update in paralel for all MDs
for MD in $MD_List; do
	MD_Index=$(( $MD_Index + 1 ))
	MD_DeviceID=$(Field 1 "$MD")
	MD_IP=$(Field 2 "$MD")

	if [[ -z "$MD_IP" ]]; then
		echo "Warning: Device $MD_DeviceID is not associated to any IP address."
		continue
	fi

	MD_FsRoot="$MD_FsPath/$MD_IP"

	chroot $MD_FsRoot apt-get update &
	MD_Update_PID[$MD_Index]=$!
done

## Wait for MD's to finish apt-get update
for i in `seq 1 $MD_Index`; do
	wait ${MD_Update_PID[$i]}
done

## Download the updates to the core
dpkg --forget-old-unavail
apt-get -f -y --download-only dist-upgrade 1>/dev/null
apt-get autoclean

## Download the updates to the MDs
for MD in $MD_List; do
	MD_DeviceID=$(Field 1 "$MD")
	MD_IP=$(Field 2 "$MD")

	if [[ -z "MD_IP" ]]; then
		echo "Warning: Device $MD_DeviceID is not associated to any IP address."
		continue
	fi

	MD_FsRoot="$MD_FsPath/$MD_IP"
	
	## Sync MD from Core
	archiveSyncMDfromCORE $MD_FsRoot

	## Donwnload other packages that where not found in the core's cache
	chroot $MD_FsRoot dpkg --forget-old-unavail
	chroot $MD_FsRoot apt-get -f -y --download-only dist-upgrade

	## Put our downloaded packages to core so other MDs can use them
	archiveSyncCOREfromMD $MD_FsRoot
done

## Put a flag so MD's and CORE to know that an updates are available
UpdatesOkStamp=$(date +%s)
touch /usr/pluto/var/Updates/UpdatesOk.flag
echo $UpdatesOkStamp > /usr/pluto/var/Updates/UpdatesOk.flag

## Remove our lock
rm -f /usr/pluto/var/Updates/DownloadUpdates.lock

