#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

module=$(basename $0)

## Lock
mkdir -p /usr/pluto/locks
lock=/usr/pluto/locks/DiskSpaceMonitor.lock

if [ -f $lock ]; then
        Logging $TYPE $SEVERITY_WARNING $module "Lock file ($lock) already exists"
        exit
fi

Logging $TYPE $SEVERITY_NORMAL $module "Locking $module"
touch $lock


function cleanRootFS { }
function cleanHomeFS { }

## Check system root partitions for free space (/)
rootDevice=$( cat /etc/fstab  | awk '{ if ($2 == "/") { print $1 } }' )
dfOutput=$(df $rootDevice | grep -v "^Filesystem")
rootUsed=$(echo $dfOutput | awk '{ print $5 }'| cut -d"%" -f1)
rootFree=$(echo $dfOutput | awk '{ print $4 }')
if [[ $rootUsed -gt 95 || $rootFree -lt 204800 ]] ;then
	Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( / ) is getting full !"
fi

## Check system /home partition for free space (/home)
homeDevice=$( cat /etc/fstab | awk '{ if ($2 == "/home") { print $1 } }' )
dfOutput=$(df $homeDevice | grep -v '^Filesystem')
homeUsed=$(echo $dfOutput | awk '{ print $5 }'| cut -d"%" -f1)
homeFree=$(echo $dfOutput | awk '{ print $4 }')
if [[ $homeUsed -gt 95 || $homeFree -lt 204800 ]] ;then
	Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( /home ) is getting full !"
	
fi

## Check Pluto Storage device for free space
TPL_BUFFALO_HDHG300LAN=1794
TPL_GENERIC_INTERNAL_DRIVE=1790

Q="SELECT PK_Device, Description  FROM Device WHERE FK_DeviceTemplate IN ($TPL_GENERIC_INTERNAL_DRIVE, $TPL_BUFFALO_HDHG300LAN) AND FK_Device_ControlledVia=$PK_Device"
StorageDevices=$(RunSQL "$Q")
for Device in $StorageDevices; do
        Device_ID=$(Field 1 "$Device")
        Device_Description=$(Field 2 "$Device")
        Device_MountPoint="/mnt/device/$Device_ID"

	## Check filesystem and force autmount to mount the device
	touch $Device_MountPoint/.check_mounted
	if [[ "$?" == 0 ]] ;then
		Device_isAccesible=1
		rm $Device_MountPoint/.check_mounted
	else
		Device_isAccesible=0
	fi

	## Check filesystem is mounted (paranoia check)
	mount $Device_MountPoint 2&>1 > /dev/null
	if [[ "$?" != 32 ]]; then
		Device_isMounted=0
	else
		Device_isMounted=1
	fi
	
	if [[ $Device_isAccesible != "1" || $Device_isMounted != "1" ]] ;then
		Logging $TYPE $SEVERITY_CRITICAL $module "Filesystem ( $Device_MountPoint ) is not accesible [m$Device_isMounted|w$Device_isAccesible]"
		continue
	fi

	## Check the space on the drive
	dfOutput=$(df $Device_MountPoint | grep -v '^Filesystem')
	Device_DiskUsed=$(echo $dfOutput | awk '{ print $5 }' | cut -d"%" -f1)
	Device_DiskFree=$(echo $dfOutput | awk '{ print $4 }')

	## FIXME: Add code to notofy someone about free space
	Logging $TYPE $SEVERITY_NORMAL  $module "Filesystem ( $Device_MountPoint ) is $Device_DiskUsed%% full having $(($Device_DiskFree / 1024))MB free"
done

## Unlock
rm $lock
