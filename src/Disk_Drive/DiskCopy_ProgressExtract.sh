#!/bin/bash

OrbiterDev="$1"

lastline=
while read line; do
	lastline="$line"
	if [[ "$line" == "Error: "* ]]; then
		if [[ -n "$OrbiterDev" ]]; then
			/usr/pluto/bin/MessageSend localhost 0 "$OrbiterDev" 1 741 10 "ripping warning" 159 53 9 "Errors were encountered while ripping the disk. The bad sectors will be skipped and replaced with zeros, but if the disk is very scratched, it may take a long while for the operation to complete. If you don't want to wait, you can cancel the operation from the 'Pending tasks' screen|OK|Pending tasks" 137 "-targetType device 0 $OrbiterDev 1 4|-targetType device 0 $OrbiterDev 1 741 159 82" 163 "ripping"
		fi
	elif [[ "$line" == "Position: "*"; Progress: "*"%" ]]; then
		Percent=${line##*Progress: }
		Percent=${Percent%%%*}
		echo "$Percent"
	fi
done
echo "$lastline" >/tmp/rip_message
