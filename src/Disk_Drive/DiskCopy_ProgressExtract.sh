#!/bin/bash

lastline=
while read line; do
	lastline="$line"
	if [[ "$line" == "Error: "* ]]; then
		echo "error"
	elif [[ "$line" == "Position: "*"; Progress: "*"%" ]]; then
		Percent=${line##*Progress: }
		Percent=${Percent%%%*}
		echo "$Percent"
	fi
done
echo "$lastline" >/tmp/rip_message
