#!/bin/bash

lastline=
while read line; do
	lastline="$line"
	if [[ "$line" != "Position: "*"; Progress: "*"%" ]]; then
		continue
	fi
	Percent=${line##*Progress: }
	Percent=${Percent%%%*}
	echo "$Percent"
done
echo "$lastline" >/tmp/rip_message
