#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Xdisplay=$Display
for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--display) ((i++)); Xdisplay="${!i#:}" ;;
		*) echo "Unknown parameter: '${!i}'" ;;
	esac
done

if [[ -z "$Xdisplay" ]]; then
	if [[ "$DISPLAY" == :* ]]; then
		Xdisplay="${DISPLAY#:}"
	else
		Xdisplay=0
	fi
fi

XPID=$(</var/run/plutoX$Xdisplay.pid)

kill "$XPID"
