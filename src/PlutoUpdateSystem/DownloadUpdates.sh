#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

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

## Make sure we don't run more than once
UpdatesDownloadingStamp=$(date +%s)
if [[ -f /usr/pluto/var/Updates/DownloadUpdates.lock ]]; then
	exit 0;
else
	echo $UpdatesDownloadingStamp > /usr/pluto/var/Updates/DownloadUpdates.lock
fi
							
## Start Logging this
echo "Download Packages Started (see /var/log/pluto/DownloadPackages.log for debuging)"
touch /var/log/pluto/DownloadPackages.newlog
mv /var/log/pluto/DownloadPackages.newlog /var/log/pluto/DownloadPackages.log > /dev/null 2>&1
exec 6>&1
exec > /var/log/pluto/DownloadPackages.newlog 2>&1
echo "##  Updates Download Started at $(date -R)"

## Check to see if we are in offline mode (pluto.conf)
if [[ $OfflineMode == "true" ]]; then
	echo "## Offline mode is activated. There's no need to run, i'm exiting"
	exit 0
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

## First check if the core realy needs to upgrade any packages
Count=$(apt-get -f -y -s dist-upgrade | egrep -c '^Inst |^Conf ')

if [[ "$Count" != "0" ]]; then	
	## Put a flag so MD's and CORE to know that an updates are available
	UpdatesOkStamp=$(date +%s)
	touch /usr/pluto/var/Updates/UpdatesOk.flag
	echo $UpdatesOkStamp > /usr/pluto/var/Updates/UpdatesOk.flag

	## Inform about the updates that are available for the core
	/usr/pluto/bin/NotifyUpdates.sh $PK_Device	

else
	## If the core has no updates, check if any of the MD's have
	for MD in $MD_List; do
        	MD_DeviceID=$(Field 1 "$MD")
        	MD_IP=$(Field 2 "$MD")

        	if [[ -z "MD_IP" ]]; then
                	continue
        	fi

        	MD_FsRoot="$MD_FsPath/$MD_IP"

		# Check if this particular MD is up
		MD_IsUp="No"
		if ssh $MD_IP ps -A | grep -q Orbiter; then
			MD_IsUp="Yes"
		fi

		# Check if we have any packages to install on this MD
		Count=$(apt-get -f -y -s dist-upgrade | egrep -c '^Inst |^Conf ')

		# Send a message to inform about the updates
		if [[ "$Count" != "0" && $MD_IsUp == "Yes" ]]; then
			/usr/pluto/bin/NotifyUpdates.sh $MD_DeviceID
		fi
	done
fi

## Remove our lock
rm -f /usr/pluto/var/Updates/DownloadUpdates.lock
