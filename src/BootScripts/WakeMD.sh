#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh
HostMAC="$1"

if [[ -z "$HostMAC" ]]; then
	echo "Usage: WakeMD.sh <MAC>"
	exit 1
fi

/usr/sbin/etherwake -b -i "$IntIf" "$HostMAC"
