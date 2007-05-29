#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if su - "$TheChosenOne" -c "DISPLAY=:$Display kcheckrunning"; then
	exit
fi

Old_IFS="$IFS"
IFS=:

while read user x uid gid rest; do
	if ((uid < 1000 || uid >= 10000)); then
		continue
	fi
	TheChosenOne="$user"
	break
done </etc/passwd

IFS="$Old_IFS"

su - "$TheChosenOne" -c "DISPLAY=:$Display startkde" &>/dev/null </dev/null &
disown -a
