#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

diskDriveDeviceID="$1"
jobID="$2"
taskID="$3"
fileName="$4"

last_progress=0
# progress can be either int or float, from stdin
while read progress text; do
	progress=${progress%%.*} # int(progress)
	if [[ "$progress" != "$last_progress" ]]; then
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "p" 256 "$progress" 13 "$fileName" >/dev/null
		last_progress="$progress"
	fi
	#echo "$progress"
done
