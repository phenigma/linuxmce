#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

diskDriveDeviceID="$1"
filename="$2"
name="$3"
owner="$4"

last_progress=0
# progress can be either int or float, from stdin
while read progress text; do
	progress=${progress%%.*} # int(progress)
	if [[ "$progress" != "$last_progress" ]]; then
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" -1000 2 35 35 "$filename" 20 7 13 "$name" 30 "$progress" 26 "$owner" >/dev/null
		last_progress="$progress"
	fi
	#echo "$progress"
done
