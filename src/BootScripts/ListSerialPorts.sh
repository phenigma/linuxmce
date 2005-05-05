#!/bin/bash

if [[ -f /proc/tty/driver/serial ]]; then
	Ports=$(cat /proc/tty/driver/serial | grep -v unknown | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyS$i"
	done
fi

if [[ -f /proc/tty/driver/usbserial ]]; then
	Ports=$(cat /proc/tty/driver/usbserial | grep ': ' | cut -d: -f1)
	for i in $Ports; do
		echo "/dev/ttyUSB$i"
	done
fi

shopt -s nullglob
for Port in /dev/ttyS_*; do
	echo "$Port"
done
