#!/bin/bash

set -x

Usage()
{
	echo "Syntax: $1 <IP>"
}

MakeChanges()
{
	shopt -s nullglob

	sed_cmd='s/^EnableGenerator=.*$/EnableGenerator=1/; s/^DelayMin=.*$/DelayMin=500/; s/^DelayMax=.*$/DelayMax=700/'
	for i in "" $(find /home/diskless -mindepth 1 -maxdepth 1 -type d 2>/dev/null); do
		sed -i "$sed_cmd" "$i"/etc/Orbiter.conf 2>/dev/null
	done
	/usr/pluto/bin/MessageSend localhost 0 -1000 7 1
	return 0
}

Core="$1"

if [[ -z "$Core" ]]; then
	Usage "$0"
	exit
fi

if [[ "$Core" == local ]]; then
	MakeChanges
else
	File=/home/update_simulator.sh
	cat "$0" | ssh root@"$Core" "cat >'$File'; chmod +x '$File'; '$File' local" || echo "Failed"
fi
