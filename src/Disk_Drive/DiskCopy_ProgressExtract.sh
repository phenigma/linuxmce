#!/bin/bash

OrbiterDev="$1"

lastline=
while read line; do
	lastline="$line"
	if [[ "$line" == "Error: "* ]]; then
		if [[ -n "$OrbiterDev" ]]; then
			/usr/pluto/bin/MessageSend localhost 0 "$OrbiterDev" 1 741 10 "ripping warning" 159 53 9 "ripping -- warning ... bad sectors... etc|OK" 137 "" 163 "ripping"
		fi
	elif [[ "$line" == "Position: "*"; Progress: "*"%" ]]; then
		Percent=${line##*Progress: }
		Percent=${Percent%%%*}
		echo "$Percent"
	fi
done
echo "$lastline" >/tmp/rip_message
