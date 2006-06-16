#!/bin/bash

if [[ ! -d /sys/class/video4linux ]]; then
	exit 1
fi

shopt -s nullglob

for Dev in /sys/class/video4linux/*; do
	devV4L=/dev/"${Dev#/sys/class/video4linux/}"
	if [[ "$devV4L" != /dev/video* ]]; then
		continue
	fi
	Model=$(cat "$Dev"/model 2>/dev/null)
	Name=$(cat "$Dev"/name 2>/dev/null)
	#UniqID=$(readlink "$Dev"/device | sed 's,^.*/\(usb.*\).*$,\1,g')
	UniqID=$(readlink "$Dev"/device | sed 's,^.*/\(pci.*\).*$,\1,g')
	echo "$UniqID $devV4L $Model:$Name"
done
