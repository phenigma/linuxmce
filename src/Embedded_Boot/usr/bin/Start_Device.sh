#!/bin/sh

Device="$1"
Cmd="$2"

if [[ -z "$Device" ]] || [[ -z "$Cmd" ]]; then
	exit
fi

if [[ -n "$(ps | grep -q "Start_Device.sh $Device" | grep -v grep)" ]]; then
	exit
fi

mkdir -p /tmp/log/pluto
while [[ ! -f /var/run/pluto.stop ]]; do
	/usr/bin/"$Cmd" -r 127.0.0.1 -d "$Device" -l /tmp/log/pluto/"$Device-$Cmd.log" 2>&1
	sleep 8
done
