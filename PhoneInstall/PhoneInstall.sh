#!/bin/bash

#$0 <MAC> <path_to_sis>

if [ "$#" -ne "2" ]; then
	echo "Usage: $0 <MAC> <path_to_sis>"
	exit
fi

MAC="$1"
SIS="$2"

#TODO: check if MAC is valid

if [ ! -f "$SIS" -o ! -r "$SIS" ]; then
	echo "sis file '$SIS' not found or not accessible"
	exit
fi

#/usr/pluto/bin/ussp-push /dev/rfcomm0 "$SIS" PlutoMO.sis

(echo "c"; echo "p"; echo "$SIS PlutoMO.sis"; echo "q") | obex_test -b $MAC 9 1>/dev/null
