#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

diskDriveDeviceID="$1"
filename="$2"

# progress can be either int or float, from stdin
while read progress text; do
	progress=${progress%%.*} # int(progress)
	/usr/pluto/bin/MessageSend 10.2.1.142 "$diskDriveDeviceID" -1000 2 35 35 "$filename" 20 7 13 "name" 30 "$Percentage" > null
	#echo "$progress"
	#/usr/pluto/bin/MessageSend $DCERouter
done
