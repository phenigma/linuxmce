#!/bin/bash

[[ "$#" -eq 3 ]] || exit 1

IP="$1"
Port="$2"
Dev="$3"
DevName="${Dev##*/}"

Lock="/usr/pluto/locks/gc100-serial-$DevName"

if [[ -f "$Lock" ]]; then
	PID="$(cat "$Lock")"
	[[ -d "/proc/$PID" ]] && 2>/dev/null grep -aF "$(basename $0)" "/proc/$PID/cmdline" | grep -qaF "$DevName" && exit
fi

echo $$ >"$Lock"
while :; do
	socat -v TCP4:"$IP":"$Port" PTY,link="$Dev",echo=0,icanon=0,raw >>"/var/log/pluto/socat_$DevName".log 2>&1
	sleep 1
done
