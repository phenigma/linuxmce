#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Action=$1
Value=$2

function getValue {
	if [[ "$OfflineMode" == "" ]]; then
		/usr/pluto/bin/OfflineMode.sh set "false"
		echo "false"
	else
		echo $OfflineMode
	fi
}

function setValue {
	local Value=$1

	if [[ "$Value" != "true" ]]; then
		Value="false"
	fi

	ConfSet "OfflineMode" "$Value"

	## Remove any existing /usr/pluto/var/apt.conf.offline
	rm -f /usr/pluto/var/apt.conf.offline

	## If we are in offline mode, put this file there so *apt* will know to not connect
	## to the internet. This is not magic, a symlink in /etc/apt/apt.conf.d/ is pointing
	## to this file. The fun part is that /usr/pluto/var is shared between mds and core
	## resulting in a option set to all machines. 
	if [[ "$Value" != "true" ]]; then
		cp /usr/pluto/templates/apt.conf.offline.tmpl /usr/pluto/var/apt.conf.offline
	fi
	
	## Set the options to MD's also
	for DlDir in /usr/pluto/diskless/*; do 
		chroot $DlDir bash -c "source /usr/pluto/bin/ConfigOps.sh && ConfSet OfflineMode $Value"
	done
	
}

case "$Action" in
        get)
                getValue
        ;;

        set)
                setValue $Value
        ;;

        *)
                echo "Usage $0 <set VALUE | get>"
                exit 0
        ;;
esac

