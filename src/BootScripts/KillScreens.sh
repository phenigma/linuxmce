#!/bin/bash

. /usr/pluto/bin/pluto.func

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Killing these processes: $*"

Tab="$(echo -e "\t")"
for Process in "$@"; do
	Pids="$Pids $(screen -ls | grep -F $Process | cut -d. -f1 | cut -d"$Tab" -f2)"
done

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Killing these PIDs: $Pids"

for Pid in $Pids; do
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Killing PID $Pid"
	kill -9 $Pid 2>&1
done

screen -wipe &>/dev/null
Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Killing finished"
