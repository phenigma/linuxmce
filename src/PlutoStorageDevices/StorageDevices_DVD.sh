#!/bin/bash

. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

Computer="$1"
BlockDevice="$2"
Action="$3"

WorkDir="/mnt/optical"

function Log {
	echo "$(date -R) $$ $*" >&2 
	echo "$(date -R) $$ $*" >> /var/log/pluto/StorageDevices_DVD.log
}

mkdir -p "$WorkDir"
Log "Called $0 $*"

## If the dvb drive is from this computer
if [[ "$Action" == "start" ]] ;then
	if [[ "$Computer" == "$PK_Device" ]] ;then

		## Test to see if the block device exists
		if [[ ! -b "/dev/${BlockDevice}" ]] ;then
			Log "Can't find block device '/dev/${BlockDevice}'"
			exit 1
		fi

		## Test to see if the block device contains a mountable dvd
		## by checking it's filesystem to be UDF or ISO9660
		if ! file -s -L "/dev/${BlockDevice}" | grep -q "UDF\|ISO" ;then
			Log "Filesystem on '/dev/${BlockDevice}' is not UDF or ISO9660"
			exit 1
		fi
		
		## Mount it where it should be
		mkdir -p "${WorkDir}/${Computer}_${BlockDevice}"
		if [[ "$(cat /proc/mounts  | grep "^/dev/${BlockDevice} " | cut -d' ' -f2)" != "$WorkDir/${Computer}_${BlockDevice}" ]] ;then
			umount -lf "/dev/${BlockDevice}" 2>/dev/null
			
			if ! mount -t auto -o user,noauto "/dev/${BlockDevice}" "$WorkDir/${Computer}_${BlockDevice}" ;then
				Log "Cannot mount /dev/${BlockDevice}"
				exit 1
			fi
		else
			Log "Device ${BlockDevice} was allready mounted"
		fi

		## Add a samba share for it so it'll be accesible by other computers too
		SambaSection="
		[Optical_${BlockDevice}\$]
		comment = Optical Drive ${BlockDevice}
		browsable = yes
		writable = no
		path = ${WorkDir}/${Computer}_${BlockDevice}
		public = no
		guest ok = no
		force user = root
		force group = root
		"

		PopulateSection "/etc/samba/smb.conf" "Optical_Drive_${BlockDevice}" "$SambaSection"
		if [[ "$(pidof smbd)" == "" ]] ;then
			invoke-rc.d samba start &>/dev/null
		else
			invoke-rc.d samba reload &>/dev/null
		fi

	## If is from another computer
	else
		## Umount if firs, nobody should use it at this point althought is lazy
		umount -lf "${WorkDir}/${Computer}_${BlockDevice}" 2>/dev/null

		## Get the IP Address of the core
		IPaddress=$(RunSQL "SELECT IPaddress from Device WHERE PK_Device = '$Computer' LIMIT 1")
		if [[ "$IPaddress" == "" ]] ;then
			Log "Cannot find IP address associated with device '$Computer'"
			exit 1
		fi

		## Tell that computer to enable the dvd drive
		if ! ssh root@"$IPaddress" "/usr/pluto/bin/StorageDevices_DVD.sh $1 $2 $3" ;then
			Log "Cannot comunicate with $Computer to enable DVD Drive sharing but continue anyway"
		fi

		mkdir -p "${WorkDir}/${Computer}_${BlockDevice}"

		## Mount / Remount the dvd drive
		if ! mount -t "smbfs" -o "ro,port=139,credentials=/usr/pluto/var/sambaCredentials.secret" "//$IPaddress/Optical_${BlockDevice}\$" "${WorkDir}/${Computer}_${BlockDevice}" ;then
			Log "Cannot mount the remote DVD"
			exit 1
		fi

		Log "Mounted successfull"
	fi

elif [[ "$Action" == "stop" ]] ;then

	if [[ "$Computer" != "$PK_Device" ]] ;then
		umount -lf "${WorkDir}/${Computer}_${BlockDevice}"
	fi
fi
