#!/bin/bash

. /usr/pluto/bin/pluto.func

# DEBUG
set -x
LogFile="/var/log/pluto/KillScreens.log"
date >>"$LogFile"
exec 2>>"$LogFile"

Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Killing these processes: $*"

Tab="$(echo -e "\t")"
for Process in "$@"; do
	Pids="$Pids $(cd /var/run/screen/S-root/; ls -1 | grep -F $Process | cut -d. -f1 | cut -d"$Tab" -f2)"
done

Pids="$(echo "$Pids" | tr '\n' ' ')"
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Killing these PIDs: $Pids"

for Pid in $Pids; do
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Killing PID $Pid"
	kill -9 $Pid 2>&1
done

screen -wipe &>/dev/null
Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Killing finished"
