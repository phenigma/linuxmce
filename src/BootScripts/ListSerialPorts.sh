#!/bin/bash

if [[ -f /proc/tty/driver/serial ]]; then
	Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyS$i"
	done
fi

# also honor ACM tty devices
Ports=
Ports=`find /sys/devices -name '*tty*' | grep '/tty:' | grep usb | sed 's,^.*\(pci.*\)/usb[0-9]*/[0-9./-]*/[0-9]*-\([0-9.]*\):[0-9.]*/tty.*$,\1+\2,g'`
echo "${Ports# }" | tr ' ' '\n'


#if [[ -d /sys/bus/usb-serial/devices ]]; then
#	pushd /sys/bus/usb-serial/devices &>/dev/null
#	Ports=
#	for dev in *; do
#		##id=$(readlink "$dev" | sed 's,^.*/\(usb.*\)/tty.*$,\1,g')
#		id=$(readlink -f "$dev" | sed 's,^.*\(pci.*\)/usb[0-9]*/[0-9./-]*/[0-9]*-\([0-9.]*\):[0-9.]*/ttyUSB[0-9]*$,\1+\2,g')
#		Ports="$Ports $id"
#	done
#	echo "${Ports# }" | tr ' ' '\n'
#	popd &>/dev/null
#fi

shopt -s nullglob
for Port in /dev/ttyS_*; do
	echo "$Port"
done
