#!/bin/bash

if [[ -z "$DISPLAY" ]]; then
	echo "ERROR: DISPLAY variable is empty. Are you running me from xinit?"
	exit 1
fi

if [[ "${#@}" -eq 0 ]]; then
	echo "ERROR: No parameters given. The WM and its parameters are needed"
	exit 1
fi

Xdisplay="${DISPLAY#:}"
echo $$ >/var/run/plutoX"$Xdisplay".pid

if [[ -f /etc/pluto/X-PostStart.sh ]]; then
	/etc/pluto/X-PostStart.sh
fi

exec "$@"
