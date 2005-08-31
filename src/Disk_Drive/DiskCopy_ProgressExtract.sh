#!/bin/bash

while read line; do
	if [[ "$line" != "Position: "*"; Progress: "*"%" ]]; then
		continue
	fi
	Percent=${line##*Progress: }
	Percent=${Percent%%%*}
	echo "$Percent"
done
