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

