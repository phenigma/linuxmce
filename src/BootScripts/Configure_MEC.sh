#!/bin/bash

set -- $(getopt "d:i:m:" -- "$@")

while [[ "$#" -ne 0 ]]; do
	case "$1" in
		--)
			shift
			break
		;;
		-d)
			Device="$2"
			shift
		;;
		-i)
			IP="$2"
			shift
		;;
		-m)
			MAC="$2"
			shift
		;;
	esac
	shift
done

/usr/pluto/bin/Diskless_Setup.sh -a "$MAC"

echo "reboot" | nc "$IP" 100
