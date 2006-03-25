#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

trap cleanExit EXIT

function cleanExit {
	echo "## !!!! DownloadUpdate.sh: Forced Exit, Cleaning up !!!"
	rm /usr/pluto/var/Updates/DownloadUpdates.lock
}

function archiveSyncMDfromCORE {
	local MD_Root=$1
	
	chroot $MD_Root apt-get autoclean
	cp -ru /var/cache/apt/archives/* $MD_Root/var/cache/apt/archives
}

function archiveSyncCOREfromMD {
	local MD_Root=$1

	cp -ru $MD_Root/var/cache/apt/archives/* /var/cache/apt/archives
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
echo "Download Packages Started (see /var/log/pluto/DownloadPackages.newlog for debuging)"
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

## Do a cleanup in /usr/pluto/deb-cache/dists/sarge/main/binary-i386/
## Note: this code was in Start_DCERouter.sh 
echo "## Cleaning old file from /usr/pluto/deb-cache/dists/sarge/main/binary-i386/"
cd /usr/pluto/deb-cache/dists/sarge/main/binary-i386/
ThisVersion="$(dpkg -s pluto-dcerouter | grep Version)"
ThisBaseVersion=$(echo "$ThisVersion" | cut -d. -f1-4)
PermanentPkgs="pluto-kernel-upgrade"
RemovedList="0==1"
for File in pluto-*_*.deb; do
        DiskVersion="$(dpkg --info "$File" | grep Version | cut -c2-)"
        DiskBaseVersion=$(echo "$DiskVersion" | cut -d. -f1-4)
        PkgName="$(dpkg --info "$File" | grep Package | cut -d' ' -f3)"
        if [[ "$PermanentPkgs" == *"$PkgName"* ]]; then
                continue # excempt permanent packages from clean-up
        fi
        if [[ "$ThisBaseVersion" != "$DiskBaseVersion" ]]; then
                rm -f $File
                RemovedList="$RemovedList || \$0 == \"Package: $PkgName\""
        fi
done

if [ "$RemovedList" != "0==1" ]; then
        gunzip -c Packages.gz >Packages.orig
        awk '
                BEGIN { Flag = 0 }
                '"$RemovedList"' { Flag = 1 }
                Flag == 0 { print }
                $0 == "" { Flag = 0 }
        ' Packages.orig >Packages.new
        gzip -9c Packages.new >Packages.gz
        rm Packages.new Packages.orig
        apt-get update
fi
cd -



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
echo "## Running apt-get update for MDs"
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
echo "## Downloading updates to the core"
dpkg --forget-old-unavail
apt-get -f -y --download-only dist-upgrade 1>/dev/null
apt-get autoclean

## Download the updates to the MDs
for MD in $MD_List; do
	MD_DeviceID=$(Field 1 "$MD")
	MD_IP=$(Field 2 "$MD")

	echo "## Downloading update to the MD - $MD_IP"
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
