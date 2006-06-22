#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/LockUtils.sh

while ! TryLock "Diskless_Setup" "Diskless_RenameFS"; do
	sleep 1
done
trap 'Unlock "Diskless_Setup" "Diskless_RenameFS"' EXIT

# Parse parameters
for ((i = 1; i <= "$#"; i++)); do
	Parm="${!i}"
	case "$Parm" in
		--devid) ((i++)); DevID="${!i}" ;;
		--oldip) ((i++)); OldIP="${!i}" ;;
		--newip) ((i++)); NewIP="${!i}" ;;
		--oldmac) ((i++)); OldMAC="${!i//-/:}"; OldMAC="$(echo "$OldMAC" | tr 'A-Z' 'a-z')" ;;
		--newmac) ((i++)); NewMAC="${!i//-/:}"; NewMAC="$(echo "$NewMAC" | tr 'A-Z' 'a-z')" ;;
	esac
done

# The Device ID of the MD must be supplied
if [[ -z "$DevID" ]]; then
	echo "Device ID is empty or not supplied"
	exit 1
fi

ChangesToBeMade=n
# Rename MD filesystem and TFTP directories if needed
if [[ "$OldIP" != "$NewIP" ]]; then
	if [[ -n "$OldIP" ]]; then
		if [[ -z "$NewIP" ]]; then
			# allocate new IP if new IP is empty, since this is going to be allocated anyway in Diskless_Setup
			# doing that here, prevents a "MD got deleted" symptom, because it was renamed
			NewIP="$(/usr/pluto/bin/PlutoDHCP.sh -d "$DevID" -a)"
		fi
#		mv -f /usr/pluto/diskless/{"$OldIP","$NewIP"}
#		rm -rf /tftpboot/"$OldIP"
	fi
	ChangesToBeMade=y
fi

# Remove MD TFTP boot files if needed
if [[ "$OldMAC" != "$NewMAC" ]]; then
	if [[ -n "$OldMAC" ]]; then
		lcdOldMAC=$(echo ${OldMAC//:/-} | tr 'A-Z' 'a-z')
		rm -f /tftpboot/pxelinux.cfg/01-"$lcdOldMAC"
	fi
	ChangesToBeMade=y
fi

if [[ "$ChangesToBeMade" == y ]]; then
	# set the "Need to configure" flag of the MD
	Q="UPDATE Device SET NeedConfigure='1' WHERE PK_Device='$DevID'"
	RunSQL "$Q"

	# Run other scripts
	# they will do the following actions, if applicable:
	# - create /tftpboot/pxelinux.cfg/01-"$lcdNewMAC"
	# - create /tftpboot/"$lcdNewIP"
	# - update and restart DHCP
	# and other actions that don't have to be duplicated here
	/usr/pluto/bin/Diskless_Setup.sh --skiplock
	/usr/pluto/bin/DHCP_config.sh
	/usr/pluto/bin/Diskless_ExportsNFS.sh
fi
