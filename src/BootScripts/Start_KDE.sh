#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

Old_IFS="$IFS"
IFS=:

DEVICEDATA_Diskless_Boot=9

IsDiskless=$(GetDeviceData "$PK_Device" "$DEVICEDATA_Diskless_Boot")

if [[ "$IsDiskless" == 1 ]]; then
	TheChosenOne=root
else
	while read user x uid gid rest; do
		if ((uid < 1000 || uid >= 10000)); then
			continue
		fi
		TheChosenOne="$user"
		break
	done </etc/passwd
fi

IFS="$Old_IFS"

if su - "$TheChosenOne" -c "DISPLAY=:$Display kcheckrunning"; then
	exit
fi

su - "$TheChosenOne" -c "DISPLAY=:$Display startkde" &>/dev/null </dev/null &
disown -a
