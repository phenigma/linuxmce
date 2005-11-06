#!/bin/bash

if [[ -f /proc/tty/driver/serial ]]; then
	Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyS$i"
	done
fi

if [[ -d /sys/bus/usb-serial/devices ]]; then
	pushd /sys/bus/usb-serial/devices &>/dev/null
	Ports=
	for dev in *; do
		id=$(readlink "$dev" | sed 's,^.*/\(usb[^/]*\)/\([^/]*\)/\([^/]*\)/.*$,\1/\2/\3,g')
		Ports="$Ports $id"
	done
	echo "${Ports# }" | tr ' ' '\n'
	popd &>/dev/null
fi

shopt -s nullglob
for Port in /dev/ttyS_*; do
	echo "$Port"
done
